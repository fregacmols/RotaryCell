# System Architecture

RotaryCell keeps the telephone itself analog while adding cellular service and electronic control around it.

## Major subsystems

**Original telephone** — handset, rotary dial, switch-hook, mechanical ringer, network block, and internal telephone wiring remain in use.

**Silvertel AG1171** — provides the subscriber-line interface to the telephone, including line operation, switch-hook sensing, and ringing capability.

**LilyGO T-A7670G-S3** — combines the ESP32-S3 controller, A7670 cellular modem, charging support, and cellular audio interface.

**Audio and Reset A4 PCB** — conditions transmit and receive audio and includes hardware intended to support modem power-cycle recovery.

**AG1171 carrier PCB** — mounts and interfaces the SLIC and associated power/control connections.

**21700 Li-ion cell** — provides portable power. Charging is supplied through the repurposed telephone jack using regulated 5 V on designated pins.

## Signal flow

The ESP32 is not in the handset voice-audio path. Telephone audio remains analog through the original telephone network and SLIC, while the ESP32 handles hook state, rotary dialing, modem control, generated telephone tones, diagnostics, and other control functions.

More detailed diagrams and photographs will be added as the documentation build progresses.
