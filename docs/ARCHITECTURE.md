# Architecture

## Preservation principle

RotaryCell is an internal, reversible interface. The original telephone remains electrically and mechanically intact. The project connects to existing terminals, mechanisms, and jacks rather than replacing the dial, ringer, network block, or handset wiring.

## Functional partitions

### LilyGO T-A7670G-S3 Standard

- Runs the ESP32-S3 firmware
- Hosts the A7670 cellular modem
- Provides cellular microphone and speaker audio connections
- Charges and monitors the 21700 cell
- Provides GPIO for AG1171 control, switch-hook sensing, dial-tone PWM, and the planned hardware-reset trigger

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

- A single protected 21700 cell connects directly to the LilyGO battery pads through a harness.
- The LilyGO's original 18650 holder is removed.
- The telephone's RJ11 jack is used only to deliver regulated 5 V charging power on the designated pins.

## Firmware GPIO baseline

Firmware v0.10.4 defines:

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

The A4 reset-trigger GPIO is not assigned in v0.10.4.
