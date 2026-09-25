# Building RotaryCell firmware

The illustrated build guide uses:

`firmware/current/ESP-RotaryCell_v0.11.1-dev/ESP-RotaryCell_v0.11.1-dev.ino`

This is the required guide firmware because it implements the GPIO35 hardware
reset and GPIO21 AG1171 power-down connections fitted to the current PCB build.
Version 0.10.4 remains in `firmware/archive/` as the earlier stable release, but
it predates those connections.

## Known working toolchain

- Arduino IDE or Arduino CLI
- Espressif ESP32 Arduino core **3.3.11**
- Board: **ESP32S3 Dev Module**
- Flash size: **16 MB**
- Flash mode: **QIO**
- CPU frequency: **240 MHz**
- USB mode: **Hardware CDC and JTAG**
- USB CDC on boot: **Enabled**
- PSRAM: **Disabled**
- Partition scheme: **Minimal SPIFFS (Large APP with OTA)** (`min_spiffs`)

The complete sanitized FQBN used for the documented builds is:

```text
esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=default,CPUFreq=240,FlashMode=qio,FlashSize=16M,PartitionScheme=min_spiffs,DebugLevel=none,PSRAM=disabled,LoopCore=1,EventsCore=1,EraseFlash=none,JTAGAdapter=default,ZigbeeMode=default
```

The firmware uses only facilities supplied by the ESP32 Arduino core: Wi-Fi, `WebServer`, LittleFS, OTA update support, serial interfaces, and ESP32 platform APIs.

## Building

1. Install ESP32 Arduino core 3.3.11.
2. Open the `.ino` file from its versioned folder.
3. Select the options above.
4. Compile before connecting the target hardware.
5. Upload by USB for an initial installation, or export an application binary for a later OTA update.

## Prebuilt file

`firmware/prebuilt/RotaryCell_v0.11.1-dev_OTA.bin` is the application-only OTA
binary for the current guide firmware. It is intended for updates through the
RotaryCell maintenance page and is **not** a complete merged factory-flash
image. Initial installation is made from source over USB. The older v0.10.4
binary and source package remain alongside it for existing installations.

Build directories, maps, ELF files, local Arduino paths, and other reproducible compiler output are deliberately excluded from the repository.

