# Building the current firmware

The active sketch is:

`firmware/current/ESP-RotaryCell_v0.10.4/ESP-RotaryCell_v0.10.4.ino`

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

The complete sanitized FQBN used for the archived build is:

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

`firmware/prebuilt/ESP-RotaryCell_v0.10.4.ino.bin` is the application-only binary produced from the archived v0.10.4 source. It is suitable for the firmware's web OTA updater when the target already has a compatible OTA partition table. It is **not** a complete merged factory-flash image.

Build directories, maps, ELF files, local Arduino paths, and other reproducible compiler output are deliberately excluded from the repository.

