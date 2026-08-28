# Firmware

The current firmware baseline is [`current/ESP-RotaryCell_v0.10.4`](current/ESP-RotaryCell_v0.10.4/).

## Build environment used

- Arduino ESP32 core: **3.3.11**
- Board target: **ESP32S3 Dev Module**
- CPU: **240 MHz**
- Flash mode: **QIO**
- Flash size: **16 MB**
- Partition scheme: **Minimal SPIFFS with OTA** (`min_spiffs`)
- USB mode: **Hardware CDC**
- CDC on boot: **Enabled**
- PSRAM: **Disabled**

The firmware uses WiFi, WebServer, LittleFS, Update, and other libraries supplied with the ESP32 Arduino core. No separate third-party Arduino library was required for the v0.10.4 build.

Open `current/ESP-RotaryCell_v0.10.4/ESP-RotaryCell_v0.10.4.ino` in Arduino IDE, select the settings above, and compile or upload normally. The complete FQBN is recorded in the root [BUILDING.md](../BUILDING.md).

## Configuration and security

Pin assignments and timing constants are in `Config.h`. The development maintenance network uses:

- SSID: `ESP-RotaryCell`
- Default password: `rotarycell`

Change `WIFI_AP_PASSWORD` before deploying the phone where untrusted people could connect. Maintenance Wi-Fi normally shuts itself off after ten minutes.

## Hardware-reset integration

Version 0.10.4 can attempt an AT-command/software modem recovery through service code `9999`, but it predates the Audio and Reset A4 hardware trigger. A later firmware revision must assign a suitable free GPIO, generate the required trigger pulse, and prevent an unintended reset during boot.

Do not guess the reset connector pinout from this document; use the ordered A4 schematic as the authority.
