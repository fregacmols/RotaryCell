#pragma once
#include <Arduino.h>

class Modem {
public:
  enum class CallResult {
    NONE,
    NO_CARRIER,
    BUSY,
    NO_ANSWER,
    NO_DIALTONE
  };
  void begin();
  void update();

  void requestStatus();
  void requestClock();
  void setClock(time_t utcEpoch, int timezoneOffsetMinutes);
  void sendRaw(const String& command);
  void pulsePowerKey();
  String diagnosticSnapshot(const char* label);
  bool restartAndRecover();
  void answerIncomingCall();
  void hangUp();
  void dialOutgoingCall(const String& number);

  bool isOnline() const;
  bool simReady() const;
  bool isRegistered() const;
  bool isIncomingCall() const;
  bool isCallActive() const;
  bool ringIndicatorActive() const;

  int rssi() const;
  const String& operatorName() const;
  const String& callerId() const;
  const String& simStatus() const;
  const String& registrationText() const;

  bool takeIncomingCallStarted();
  bool takeIncomingCallEnded();
  bool takeCallResult(CallResult& result);
  bool takeNetworkTime(time_t& utcEpoch, int& timezoneQuarterHours);
  static const char* callResultName(CallResult result);

private:
  bool waitForAT(uint32_t timeoutMs);
  void configureModem();
  void configureVoiceAudio();
  void readSerial();
  void processLine(String line);
  void parseClip(const String& line);
  void parseCpin(const String& line);
  void parseCsq(const String& line);
  void parseOperator(const String& line);
  void parseRegistration(const String& line);
  void parseClock(const String& line);
  void setIncomingCall(bool active);
  String collectCommand(const String& command, uint32_t timeoutMs = 1200);

  HardwareSerial _serial{1};
  String _lineBuffer;

  bool _online = false;
  bool _simReady = false;
  bool _registered = false;
  bool _incomingCall = false;
  bool _callActive = false;
  bool _incomingStartedEvent = false;
  bool _incomingEndedEvent = false;
  CallResult _callResultEvent = CallResult::NONE;

  int _rssi = -1;
  String _operatorName;
  String _callerId;
  String _simStatus = "UNKNOWN";
  String _registrationText = "UNKNOWN";
  bool _seenCereg = false;

  uint32_t _lastRingAt = 0;

  bool _networkTimeEvent = false;
  bool _clockRequestPending = false;
  time_t _networkUtcEpoch = 0;
  int _networkTimezoneQuarterHours = 0;
};
