# Build Notes — v0.9

## Reference documents

- `../Hardware/ESP-CellCore_Service_Schematic_RevB.pdf` — primary service schematic
- `../Hardware/Complete_Wiring_Diagram.pdf` — expanded wiring reference
- `../Hardware/Transformer_Audio_Interface.pdf` — audio-board detail

## Essential wiring rules

- Do not ground either LilyGO `SPEK` conductor.
- Do not ground either LilyGO `MIC` conductor on the LilyGO side of the transformers.
- Ground-reference only the AG1171-side transformer windings as shown.
- AG1171 pin 9 `VIN` is the cell-to-rotary receive input.
- AG1171 pin 10 `VOUT` is the rotary-to-cell transmit output.
- AG1171 `PD` remains unconnected.
- AG1171 pins 6, 7, 8, and 11 remain unconnected.
- This is an internal, self-contained TIP/RING circuit only. Do not connect it to PSTN or house wiring.

## Firmware pin assignments

| Function | ESP32-S3 GPIO |
|---|---:|
| AG1171 F/R | 15 |
| AG1171 RM | 16 |
| AG1171 SHK | 37 |
| Optional tone output | 36 |
| Modem RX | 5 |
| Modem TX | 4 |
| Modem RI | 6 |
| Modem DTR | 7 |
| Modem PWRKEY | 46 |
| Modem power capability | 42 |
| Battery ADC | 8 |

## Required modem audio initialization

The LilyGO external audio amplifier must be enabled. v0.9 sends the required modem GPIO and audio configuration commands in firmware. The key discovery was that calls can be connected while the physical speaker output remains silent unless the amplifier-enable GPIO is asserted.

## Initial level adjustment

1. Start VR1 and VR2 near minimum signal.
2. Place a call with the two telephones in separate rooms.
3. Raise VR1 until receive volume is usable.
4. Raise VR2 until the far-end caller hears clear speech.
5. Back VR2 down if ringing, squelch, echo, or feedback occurs.

## Battery service note

After physically installing or replacing the battery, briefly connect USB-C to wake the battery power path. Ordinary power-switch cycling does not require USB afterward.
