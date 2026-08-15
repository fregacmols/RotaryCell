# ESP-RotaryCell v0.8 Release Notes

## Status

**Functional integrated prototype.** This is the first release treated as a complete hardware-and-firmware baseline.

## Hardware Rev A

- LilyGO T-A7670G-S3 Standard with A7670G-LLSE modem
- Silvertel AG1171 SLIC
- Western Electric/AT&T Model 500
- Two 600:600-ohm audio transformers
- VR1 receive-level control
- VR2 transmit-level control
- AG1171-controlled original bell and hookswitch interface

The authoritative release schematic is `../Hardware/ESP-CellCore_Service_Schematic_RevB.pdf`.

## Firmware features

- Incoming-call detection and original bell cadence
- Answer on handset lift
- Hangup/cancel on handset replacement
- Rotary pulse decoding and digit accumulation
- Six-second number-completion timeout
- Outgoing cellular dialing
- Local block of exact 911 and 900/1-900 numbers
- Rejection of numbers shorter than seven digits
- Handling of common modem call-result messages
- USB serial diagnostics and AT terminal
- Battery-voltage monitoring
- LilyGO audio-amplifier initialization

## Important note about dial tone

The firmware includes a GPIO36 dial-tone generator, but the v0.8 physical unit does not yet have the passive tone-injection network installed. Lifting the handset enters the dial-tone state, but no local tone is heard until that circuit is added.

## Known limitations

See `Known_Issues.md` for the current list.
