# INA226 microSD development logger

This is a development-only derivative of RotaryCell firmware v0.10.4. It does
not replace the public firmware in `firmware/current`.

It retains normal RotaryCell operation while sampling the external INA226 ten
times per second and writing CSV data to the LilyGO board's microSD/TF slot.

## Hardware assumed

- LilyGO T-A7670X-S3 Standard Rev 1.0
- INA226 module connected to the QWIIC socket
- INA226 address `0x40` (A0 and A1 low, as shipped on most modules)
- INA226 module marked `R002`, meaning a 0.002-ohm shunt
- FAT32-formatted microSD card

The LilyGO schematic assigns the connections as follows:

| Function | ESP32-S3 GPIO |
|---|---:|
| QWIIC SDA | 3 |
| QWIIC SCL | 2 |
| microSD CS | 10 |
| microSD MOSI | 11 |
| microSD SCK | 12 |
| microSD MISO | 13 |

No additional Arduino library is needed. The logger uses `Wire`, `SPI`, `SD`,
and `FS`, all supplied by ESP32 Arduino core 3.3.11.

## Log files

Each boot creates the first unused filename from `POWER001.CSV` through
`POWER999.CSV`. The columns are:

```text
millis,bus_voltage_V,current_A,power_W,shunt_voltage_mV
```

Rows are buffered in RAM and flushed every five seconds. This greatly reduces
small SD writes. Sudden power loss may lose the last few seconds, but does not
write measurement data to ESP32 internal flash.

Remove the microSD card only while RotaryCell is powered off. SD writes can
briefly delay the main loop, so this development build should also be checked
for missed rotary pulses during long logging sessions.

## Startup messages

Open the USB serial console at 115200 baud. A successful start reports the CSV
filename. If either the INA226 or card is unavailable, power logging disables
itself and the normal telephone firmware continues.

The calibration provides 0.1 mA per current-register count and a measurement
range of approximately +/-3.2767 A. That is appropriate for expected RotaryCell
development loads. Reverse current is recorded as a negative value.
