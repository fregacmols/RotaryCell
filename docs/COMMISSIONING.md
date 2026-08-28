# Firmware installation and commissioning

This procedure brings up a newly assembled RotaryCell in stages. It assumes firmware v0.10.4 and the August 2026 PCB candidates.

## 1. Install the firmware

1. Follow [BUILDING.md](../BUILDING.md) for the known Arduino ESP32 core and board settings.
2. Upload v0.10.4 by USB for the initial installation.
3. Confirm the USB console reaches the normal idle state without repeated resets.

The application-only binary in `firmware/prebuilt` is intended for the firmware's OTA updater; it is not a complete factory-flash image.

## 2. First power-up

1. Use a protected cell or current-limited bench supply with correct polarity.
2. Keep the telephone line pair, AG1171 `PD` lead, and A4 reset harness disconnected.
3. Confirm stable LilyGO operation and sensible battery-voltage reporting.
4. Confirm carrier CN1 receives the protected 21700 voltage directly with correct polarity before installing or energizing the AG1171. Never apply the regulated 5 V charging input to CN1.
5. Check for abnormal heating or current draw before proceeding.

## 3. Verify controls without placing a call

1. Connect FR, RM, and SHK using the documented U3 harness.
2. Operate the hookswitch and verify `ON HOOK` / `OFF HOOK` transitions in the console.
3. Rotate the dial and confirm every pulse digit is decoded correctly.
4. Confirm GPIO36 produces dial tone and stops at the first dial pulse.

## 4. Establish cellular service

1. Install an activated, voice-capable SIM compatible with the A7670G and local carrier.
2. Attach the cellular antenna before allowing the modem to transmit.
3. Wait for the dashboard or console to report the modem online and cellular registration complete.
4. Dial `0000` to start the maintenance Wi-Fi when browser diagnostics are needed.
5. Connect to SSID `ESP-RotaryCell` using the documented default password `rotarycell`.

Change the password when the telephone will operate around people who should not have access to its modem console.

## 5. Set audio levels

1. Begin with SPK_LVL and MIC_LVL near minimum signal level, verified with a meter rather than knob direction alone.
2. Confirm `SPEK-` is floating and insulated; confirm `MIC-` reaches common ground only through the external 1 uF film/bipolar capacitor.
3. Place a test call and raise receive level until the handset is clear without distortion.
4. Raise transmit level until the remote caller hears clean audio without excessive noise, echo, or feedback.
5. Recheck both directions with the housing placed in its normal position because wire and board placement can affect noise pickup.

## 6. Functional tests

- Incoming call detection and physical bell cadence
- Answer by handset pickup and hang-up by replacement
- Seven- and ten-digit outgoing calls
- Dial, reorder, and receiver-off-hook warning tones
- Bidirectional audio quality
- Battery reporting
- Maintenance Wi-Fi, live console, persistent log, and clock synchronization
- USB and application OTA update paths

## 7. Hardware reset validation

Firmware v0.10.4 does not control the A4 reset input. Validate the reset circuit separately with the telephone and AG1171 disconnected, following [BRINGUP.md](BRINGUP.md). Only assign a firmware GPIO after confirming trigger polarity, idle state, interruption time, reliable restart, and physical-switch bypass behavior.

The AG1171 `PD` input is also present on the carrier but has no assigned v0.10.4 GPIO. Leave it disconnected until its boot-safe behavior is implemented.

## 8. Endurance test

After basic operation passes, leave the complete telephone powered for an extended test and periodically verify registration, incoming calls, outgoing calls, and event logging. The working prototype previously entered a registered-but-unusable modem state after roughly one hour; only cycling the LilyGO physical power switch restored service. Treat recovery from that condition as an explicit acceptance test for the later reset-enabled firmware.

Record the firmware version, PCB revisions, carrier/SIM, signal strength, test duration, and any reset or call failure in the project log.
