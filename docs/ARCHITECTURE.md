# Architecture

## Preservation principle

RotaryCell is an internal, reversible interface. The original telephone remains electrically and mechanically intact. The project connects to existing terminals, mechanisms, and jacks rather than replacing the dial, ringer, network block, or handset wiring.

## Functional partitions

### LilyGO T-A7670G-S3 Standard

- Runs the ESP32-S3 firmware
- Hosts the A7670 cellular modem
- Provides cellular microphone and speaker audio connections
- Charges and monitors the 21700 cell
- Provides GPIO for AG1171 control, switch-hook sensing, dial-tone PWM, and the hardware-reset trigger

### AG1171 carrier

- Mounts the Silvertel AG1171 module
- Routes logic and power between the AG1171 and LilyGO
- Connects the AG1171 line interface to the original telephone network block
- Provides ring-mode and forward/reverse control plus switch-hook feedback

### Audio and Reset A4 board

- Conditions LilyGO speaker audio for the AG1171 voice input
- Conditions AG1171 voice output for the LilyGO microphone input
- Retains adjustable transmit and receive level controls
- Provides the one-shot/high-side switching circuit for cycling the LilyGO physical power-switch path

### Battery and charging

- A single 21700 cell connects to the LilyGO `BAT` and `BATN` battery points through a harness.
- The AG1171 carrier is supplied from the LilyGO `VBAT` header pad and LilyGO system GND. Its ground must not be connected directly to `BATN`, because `BATN` is the cell side of the LilyGO's low-side battery protector.
- The LilyGO's original 18650 holder is removed.
- The current build replaces the telephone's rear connector with a printed USB-C insert. Only regulated 5 V and system GND connect to the LilyGO charging input; this remains separate from the AG1171 VPWR path.

## Firmware GPIO assignments

Version 0.10.4 is retained as the earlier stable release. The illustrated PCB
build requires v0.11.1-dev, which adds the reset and AG1171 power-down
assignments shown below.

| Function | ESP32-S3 GPIO |
| --- | ---: |
| AG1171 FR | 15 |
| AG1171 RM | 16 |
| AG1171 SHK | 37 |
| Dial-tone PWM | 36 |
| Modem RX | 5 |
| Modem TX | 4 |
| Modem RI | 6 |
| Modem DTR | 7 |
| Modem PWRKEY | 46 |
| Modem power save | 42 |
| Battery ADC | 8 |
| Hardware reset trigger | 35 (v0.11.1-dev) |
| AG1171 PD | 21 (v0.11.1-dev; high-impedance normally, LOW to power down) |

The GPIO35 reset trigger and GPIO21 AG1171 power-down control have passed testing
on assembled hardware. They remain in development-labelled firmware while
automatic modem-failure detection continues.
