# Development OTA binary

`RotaryCell_v0.11.1-dev_OTA.bin` is the Arduino application binary built from
`development/firmware/ESP-RotaryCell_v0.11.1-dev` for upload through the
RotaryCell maintenance page.

- Firmware label: `0.11.1-dev`
- File size: 1,117,840 bytes
- SHA-256: `7620BD735F53E712F4AD2067849FEE7121792C4BCB87E3CA5E9E1C7E935D0C71`
- ESP32 Arduino core: 3.3.11
- Board target: ESP32S3 Dev Module
- Flash size: 16 MB
- Partition scheme: Minimal SPIFFS with OTA (`min_spiffs`)
- USB CDC on boot: enabled
- PSRAM: disabled

This binary has the optional INA226/microSD development power logger enabled.
Phones without that hardware continue to operate, but the logger reports its
initialization failure and disables itself.
