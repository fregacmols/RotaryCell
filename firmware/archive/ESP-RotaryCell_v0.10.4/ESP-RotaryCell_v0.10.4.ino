#include "Config.h"
#include "PhoneLine.h"
#include "RingGenerator.h"
#include "BatteryMonitor.h"
#include "Modem.h"
#include "Console.h"
#include "DialToneGenerator.h"
#include "ClockManager.h"
#include "EventLog.h"
#include "MaintenanceService.h"
#include <esp_system.h>
#include "LogSerial.h"

PhoneLine phoneLine;
RingGenerator ringGenerator;
BatteryMonitor batteryMonitor;
Modem modem;
Console console(phoneLine, ringGenerator, batteryMonitor, modem);
DialToneGenerator dialTone;
ClockManager clockManager;
EventLog eventLog(clockManager);
MaintenanceService maintenance(phoneLine, batteryMonitor, modem, console, eventLog, clockManager);

bool previousOffHook = false;
String dialedNumber;
uint32_t lastDigitAt = 0;
uint32_t dialToneStartedAt = 0;
uint32_t reorderStartedAt = 0;
uint32_t lastClockRequestAt = 0;
uint32_t lastClockSyncAt = 0;
bool lowBatteryLogged = false;

enum class PhoneState {
  IDLE,
  INCOMING,
  DIAL_TONE,
  COLLECTING,
  OUTGOING,
  CONNECTED,
  REORDER,
  HOWLER
};

PhoneState phoneState = PhoneState::IDLE;

const char* phoneStateName(PhoneState state) {
  switch (state) {
    case PhoneState::IDLE:       return "IDLE";
    case PhoneState::INCOMING:   return "INCOMING";
    case PhoneState::DIAL_TONE:  return "DIAL TONE";
    case PhoneState::COLLECTING: return "COLLECTING DIGITS";
    case PhoneState::OUTGOING:   return "OUTGOING";
    case PhoneState::CONNECTED:  return "CONNECTED";
    case PhoneState::REORDER:    return "REORDER TONE";
    case PhoneState::HOWLER:     return "RECEIVER OFF HOOK";
    default:                     return "UNKNOWN";
  }
}

String logTimestampProvider() {
  return clockManager.timestamp();
}

void setPhoneState(PhoneState state) {
  if (state == phoneState) {
    return;
  }
  phoneState = state;
  maintenance.setPhoneState(phoneStateName(phoneState));
  LogSerial.print("STATE: ");
  LogSerial.println(phoneStateName(phoneState));
}

bool isBlockedNumber(const String& number, String& reason) {
  if (number == "911") {
    reason = "emergency number 911";
    return true;
  }

  // North-American premium-rate 900 service, with or without leading 1.
  if (number.startsWith("900") || number.startsWith("1900")) {
    reason = "premium-rate 900 number";
    return true;
  }

  if (number.length() < DIAL_MIN_DIGITS) {
    reason = "incomplete/short number";
    return true;
  }

  return false;
}

void clearDialing() {
  dialTone.stop();
  dialedNumber = "";
  lastDigitAt = 0;
  dialToneStartedAt = 0;
  reorderStartedAt = 0;
}

void startDialToneTreatment() {
  clearDialing();
  dialToneStartedAt = millis();
  dialTone.startDialTone();
  LogSerial.println("DIAL: ready");
  setPhoneState(PhoneState::DIAL_TONE);
}

void startReorderTreatment(const char* reason) {
  clearDialing();
  reorderStartedAt = millis();
  dialTone.startReorderTone();
  LogSerial.print("REORDER: ");
  LogSerial.println(reason);
  setPhoneState(PhoneState::REORDER);
}

void startHowlerTreatment() {
  dialTone.stop();
  reorderStartedAt = 0;
  dialTone.startHowlerTone();
  LogSerial.println("HOWLER: receiver remains off hook");
  eventLog.add("Receiver-off-hook warning tone started");
  setPhoneState(PhoneState::HOWLER);
}

void handleMaintenanceCode() {
  clearDialing();
  LogSerial.println("SERVICE: 0000 maintenance Wi-Fi toggle");
  maintenance.toggle();
  eventLog.add(maintenance.isEnabled()
                 ? "Service code 0000 enabled maintenance Wi-Fi"
                 : "Service code 0000 disabled maintenance Wi-Fi");

  // Keep the ordinary telephone interaction usable after the local service
  // code; no cellular call is ever attempted for 0000.
  if (phoneLine.isOffHook() && !modem.isCallActive() && !modem.isIncomingCall()) {
    startDialToneTreatment();
  }
}

void handleModemRecoveryCode() {
  clearDialing();
  LogSerial.println("SERVICE: 9999 modem diagnostic + recovery");
  eventLog.add("===== MODEM RECOVERY 9999 START =====");

  if (modem.isCallActive() || modem.isIncomingCall()) {
    LogSerial.println("SERVICE: 9999 refused because a cellular call is active");
    eventLog.add("9999 recovery refused: cellular call active");
    eventLog.add("===== MODEM RECOVERY 9999 END =====");
    return;
  }

  // Capture ESP-side state before touching the modem.  These lines are written
  // immediately to LittleFS so they survive the modem restart and remain
  // available even if the maintenance page is opened much later.
  eventLog.add("PRE-RESTART ESP STATE:");
  eventLog.add(String("Phone state: ") + phoneStateName(phoneState));
  eventLog.add(String("Handset: ") + (phoneLine.isOffHook() ? "OFF HOOK" : "ON HOOK"));
  eventLog.add(String("Uptime: ") + String(millis() / 1000UL) + " s");
  if (batteryMonitor.readingValid()) {
    eventLog.add(String("Battery: ") + String(batteryMonitor.volts(), 3) + " V (" +
                 String(batteryMonitor.estimatedPercent()) + "%)");
  } else {
    eventLog.add("Battery: reading unavailable");
  }

  eventLog.add("PRE-RESTART MODEM DIAGNOSTIC:");
  String pre = modem.diagnosticSnapshot("PRE-RESTART");
  eventLog.addMultiline(pre);

  eventLog.add("MODEM RECOVERY: requesting restart");
  const uint32_t started = millis();
  const bool recovered = modem.restartAndRecover();
  const uint32_t elapsedSeconds = (millis() - started + 999) / 1000;

  eventLog.add(String("MODEM RECOVERY RESULT: ") + (recovered ? "SUCCESS" : "FAILED") +
               " after " + String(elapsedSeconds) + " s");
  eventLog.add("POST-RESTART MODEM DIAGNOSTIC:");
  String post = modem.diagnosticSnapshot("POST-RESTART");
  eventLog.addMultiline(post);
  eventLog.add(String("POST-RESTART ESP STATE: phone=") + phoneStateName(phoneState) +
               ", handset=" + (phoneLine.isOffHook() ? "OFF HOOK" : "ON HOOK") +
               ", uptime=" + String(millis() / 1000UL) + " s");
  eventLog.add("===== MODEM RECOVERY 9999 END =====");

  LogSerial.print("SERVICE: 9999 recovery ");
  LogSerial.println(recovered ? "completed" : "FAILED");

  if (phoneLine.isOffHook() && !modem.isCallActive() && !modem.isIncomingCall()) {
    startDialToneTreatment();
  }
}

void updateNetworkClock() {
  time_t utcEpoch;
  int timezoneQuarterHours;
  if (modem.takeNetworkTime(utcEpoch, timezoneQuarterHours)) {
    const bool firstSync = !clockManager.isSynced();
    clockManager.setNetworkTime(utcEpoch, timezoneQuarterHours);
    lastClockSyncAt = millis();
    LogSerial.print("CLOCK: synchronized from cellular network: ");
    LogSerial.println(clockManager.displayTime());
    if (firstSync) {
      eventLog.add("Clock synchronized from cellular network");
    }
  }

  if (modem.isCallActive() || modem.isIncomingCall() || !modem.isOnline() || !modem.isRegistered()) {
    return;
  }

  const uint32_t now = millis();
  if (clockManager.isSynced()) {
    // secondsSinceSync() follows the most recent source, including browser sync,
    // so opening the dashboard does not immediately get overwritten by CCLK.
    if (clockManager.secondsSinceSync() * 1000UL >= CLOCK_RESYNC_INTERVAL_MS &&
        (lastClockRequestAt == 0 || now - lastClockRequestAt >= CLOCK_RETRY_INTERVAL_MS)) {
      modem.requestClock();
      lastClockRequestAt = now;
    }
  } else if (lastClockRequestAt == 0 || now - lastClockRequestAt >= CLOCK_RETRY_INTERVAL_MS) {
    modem.requestClock();
    lastClockRequestAt = now;
  }
}

void updateBatteryEvents() {
  if (!batteryMonitor.readingValid()) return;
  const uint8_t percent = batteryMonitor.estimatedPercent();
  if (percent <= 15 && !lowBatteryLogged) {
    eventLog.add("Low battery: " + String(percent) + "% (" + String(batteryMonitor.volts(), 3) + " V)");
    lowBatteryLogged = true;
  } else if (percent >= 25 && lowBatteryLogged) {
    eventLog.add("Battery recovered above low threshold: " + String(percent) + "%");
    lowBatteryLogged = false;
  }
}

void setup() {
  LogSerial.begin(SERIAL_BAUD);
  clockManager.begin();
  LogSerial.setTimestampProvider(logTimestampProvider);
  delay(750);

  const bool eventLogReady = eventLog.begin();
  if (!eventLogReady) {
    LogSerial.println("LOG: WARNING - LittleFS persistent event log unavailable");
  }
  eventLog.add("Boot v" + String(FIRMWARE_VERSION) + "; reset reason " +
               String(static_cast<int>(esp_reset_reason())));

  phoneLine.begin();
  ringGenerator.begin();
  batteryMonitor.begin();
  modem.begin();
  dialTone.begin();
  console.begin();
  maintenance.begin();

  previousOffHook = phoneLine.isOffHook();
  maintenance.setPhoneState(phoneStateName(phoneState));
  LogSerial.print("STATE: ");
  LogSerial.println(phoneStateName(phoneState));

  // modem.begin() already requests +CCLK as part of its initial status pass.
  lastClockRequestAt = millis();
}

void loop() {
  phoneLine.update();
  ringGenerator.update();
  batteryMonitor.update();
  modem.update();
  console.update();
  maintenance.update();
  updateNetworkClock();
  updateBatteryEvents();

  // A new cellular call starts the physical bell only while on hook.
  if (modem.takeIncomingCallStarted()) {
    LogSerial.println();
    LogSerial.println("CELLULAR: incoming call");
    eventLog.add("Incoming cellular call" + (modem.callerId().length() ? String(" from ") + modem.callerId() : String("")));
    setPhoneState(PhoneState::INCOMING);

    clearDialing();

    if (modem.callerId().length() > 0) {
      LogSerial.print("CALLER ID: ");
      LogSerial.println(modem.callerId());
    }

    if (phoneLine.isOffHook()) {
      LogSerial.println("CELLULAR: handset already off hook; answering");
      modem.answerIncomingCall();
      eventLog.add("Incoming call answered automatically because handset was already off hook");
      setPhoneState(PhoneState::CONNECTED);
    } else {
      ringGenerator.startIncomingCadence();
    }
  }

  // Stop the physical bell when the remote caller disconnects.
  if (modem.takeIncomingCallEnded()) {
    if (ringGenerator.isIncomingCadence()) {
      ringGenerator.stop();
    }
    LogSerial.println("CELLULAR: call ended");
    eventLog.add("Incoming/cellular call ended");
    if (!phoneLine.isOffHook()) {
      setPhoneState(PhoneState::IDLE);
    }
  }

  const bool offHook = phoneLine.isOffHook();

  // React only to a genuine handset pickup, not to dial-pulse breaks.
  if (offHook && !previousOffHook) {
    if (ringGenerator.isActive()) {
      ringGenerator.stop();
      LogSerial.println("RING: stopped because handset went off hook");
    }

    if (modem.isIncomingCall()) {
      modem.answerIncomingCall();
      LogSerial.println("CELLULAR: call answered; voice audio enabled");
      eventLog.add("Incoming call answered by handset pickup");
      setPhoneState(PhoneState::CONNECTED);
    } else if (!modem.isCallActive()) {
      startDialToneTreatment();
    }
  }

  // The first recognized rotary pulse silences dial tone immediately.
  if (phoneState == PhoneState::DIAL_TONE &&
      dialTone.isDialToneActive() &&
      phoneLine.pendingPulseCount() > 0) {
    dialTone.stop();
    dialToneStartedAt = 0;
    setPhoneState(PhoneState::COLLECTING);
  }

  // Consume completed rotary digits and build the outgoing number.
  uint8_t digit = 0;
  if (phoneLine.takeCompletedDigit(digit)) {
    if (offHook &&
        !modem.isIncomingCall() &&
        !modem.isCallActive() &&
        (phoneState == PhoneState::DIAL_TONE || phoneState == PhoneState::COLLECTING)) {
      dialTone.stop();

      if (dialedNumber.length() < DIAL_MAX_DIGITS) {
        dialedNumber += static_cast<char>('0' + digit);
        lastDigitAt = millis();
        setPhoneState(PhoneState::COLLECTING);

        LogSerial.print("DIAL: digit = ");
        LogSerial.println(digit);
        LogSerial.print("DIAL: number = ");
        LogSerial.println(dialedNumber);

        // v0.9 local service code. Intercept immediately and never pass it to
        // the cellular modem. Dial 0000 again while Wi-Fi is active to turn it off.
        if (dialedNumber == "0000") {
          handleMaintenanceCode();
        } else if (dialedNumber == "9999") {
          handleModemRecoveryCode();
        }
      } else {
        LogSerial.println("DIAL: maximum number length reached; digit ignored");
      }
    }
  }

  // Three seconds with no new digit means the number is complete.
  if (offHook &&
      !modem.isIncomingCall() &&
      !modem.isCallActive() &&
      phoneState == PhoneState::COLLECTING &&
      dialedNumber.length() > 0 &&
      lastDigitAt > 0 &&
      millis() - lastDigitAt >= DIAL_NUMBER_TIMEOUT_MS) {
    const String numberToDial = dialedNumber;
    dialedNumber = "";
    lastDigitAt = 0;

    LogSerial.println("DIAL: number complete after 3.0 s");
    LogSerial.print("DIAL: complete -> ");
    LogSerial.println(numberToDial);

    String blockReason;
    if (isBlockedNumber(numberToDial, blockReason)) {
      LogSerial.print("SAFETY: BLOCKED ");
      LogSerial.println(blockReason);
      LogSerial.println("DIAL: call not placed");
      eventLog.add("Blocked dial attempt: " + blockReason);
      startReorderTreatment("incomplete or blocked number");
    } else {
      LogSerial.print("MODEM: ATD");
      LogSerial.print(numberToDial);
      LogSerial.println(";");
      eventLog.add("Outgoing call placed to " + numberToDial);
      modem.dialOutgoingCall(numberToDial);
      setPhoneState(PhoneState::OUTGOING);
    }
  }

  // Permanent-signal treatment: an untouched off-hook line progresses from
  // dial tone to reorder, then to the receiver-off-hook warning tone.
  if (offHook &&
      phoneState == PhoneState::DIAL_TONE &&
      dialToneStartedAt > 0 &&
      millis() - dialToneStartedAt >= OFF_HOOK_DIAL_TONE_TIMEOUT_MS) {
    eventLog.add("Dial tone timed out with receiver off hook");
    startReorderTreatment("receiver left off hook without dialing");
  }

  if (offHook &&
      phoneState == PhoneState::REORDER &&
      reorderStartedAt > 0 &&
      millis() - reorderStartedAt >= REORDER_BEFORE_HOWLER_MS) {
    startHowlerTreatment();
  }

  // Hanging up ends an active cellular call or abandons an unfinished number.
  if (!offHook && previousOffHook) {
    clearDialing();

    if (modem.isCallActive() || modem.isIncomingCall()) {
      modem.hangUp();
      LogSerial.println("CELLULAR: call ended/cancelled because handset went on hook");
      eventLog.add("Call ended/cancelled by handset replacement");
    } else {
      LogSerial.println("DIAL: cancelled; handset on hook");
    }
    setPhoneState(PhoneState::IDLE);
  }

  Modem::CallResult callResult;
  if (modem.takeCallResult(callResult)) {
    LogSerial.print("CALL RESULT: ");
    LogSerial.println(Modem::callResultName(callResult));
    eventLog.add("Call result: " + String(Modem::callResultName(callResult)));

    if (ringGenerator.isActive()) {
      ringGenerator.stop();
    }

    if (offHook) {
      startReorderTreatment("cellular call ended or failed");
    } else {
      setPhoneState(PhoneState::IDLE);
    }
  }

  previousOffHook = offHook;
}
