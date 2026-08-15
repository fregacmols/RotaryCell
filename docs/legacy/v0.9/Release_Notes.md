# ESP-RotaryCell v0.9 Release Notes

v0.9 is a serviceability/diagnostics release based directly on the working v0.8 telephone firmware. Its purpose is to make the completed phone inspectable without opening the case or attaching a computer.

## Added

- Local rotary service code `0000` toggles maintenance Wi-Fi and is never dialed through the cellular modem.
- WPA2 access point `ESP-RotaryCell`, automatically disabled after 10 minutes.
- Local browser dashboard at `http://192.168.4.1` with battery, cellular, modem, handset, phone-state, clock, and timeout status.
- Browser console sharing the existing v0.8 command interpreter and AT-terminal mode.
- 32 KiB RAM ring buffer containing serial output from before the web client connected.
- LittleFS persistent event log, bounded to approximately 64 KiB.
- Cellular-network time synchronization using `AT+CCLK?`; log output receives real timestamps once synchronized and uptime timestamps beforehand.
- Periodic clock refresh while idle.
- Persistent low-battery transition events.

## Preserved from v0.8

The AG1171 SHK rotary/hook logic, ring generator, dial tone, incoming/outgoing call handling, six-second number completion timeout, dialing safety blocks, modem audio configuration, battery ADC monitoring, and USB serial console are otherwise unchanged.

## Maintenance credentials

- SSID: `ESP-RotaryCell`
- Default password: `rotarycell`

The password is defined in `Config.h` and should be changed if the phone will be operated where untrusted users may be nearby.
