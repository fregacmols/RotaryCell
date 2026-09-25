# Firmware

The current supported firmware is
[`current/ESP-RotaryCell_v0.11.1-dev`](current/ESP-RotaryCell_v0.11.1-dev/).
It is the firmware used by the illustrated PCB build and implements the GPIO35
hardware-reset trigger and GPIO21 AG1171 power control used by the current
Audio/Reset A4 and carrier-board wiring.

The version remains development-labelled while endurance and automatic modem
recovery work continues. The earlier stable v0.10.4 source is retained under
[`archive/ESP-RotaryCell_v0.10.4`](archive/ESP-RotaryCell_v0.10.4/).

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

The firmware uses WiFi, WebServer, LittleFS, Update, and other libraries supplied
with the ESP32 Arduino core. No separate third-party Arduino library is required.

For the illustrated build, open
`firmware/current/ESP-RotaryCell_v0.11.1-dev/ESP-RotaryCell_v0.11.1-dev.ino`
in Arduino IDE, select the settings above, and compile or upload normally. The
complete FQBN is recorded in the root
[FIRMWARE_BUILDING.md](../FIRMWARE_BUILDING.md).

## Configuration and security

Pin assignments and timing constants are in `Config.h`. The development maintenance network uses:

- SSID: `ESP-RotaryCell`
- Default password: `rotarycell`

Change `WIFI_AP_PASSWORD` before deploying the phone where untrusted people could connect. Maintenance Wi-Fi normally shuts itself off after ten minutes.

## Hardware-reset integration

Version 0.10.4 can attempt an AT-command/software modem recovery through service
code `9999`, but it predates the Audio and Reset A4 hardware trigger. Version
0.11.1-dev assigns GPIO35 to that trigger, records paired pre/post-reset
diagnostics and has successfully power-cycled assembled phones on battery. It
also controls AG1171 `PD` from GPIO21 using the required low-or-high-impedance
behavior. See [STATUS.md](../STATUS.md) and the development firmware notes for
the current validation record.
