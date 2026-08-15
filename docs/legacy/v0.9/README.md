# ESP-RotaryCell

ESP-RotaryCell converts a Western Electric/AT&T Model 500 rotary telephone into a self-contained cellular telephone while preserving the original dial, handset, hookswitch, and mechanical bell.

The current baseline is **v0.8**, a functional prototype built around:

- LilyGO T-A7670G-S3 Standard cellular board
- Silvertel AG1171 subscriber-line interface
- Western Electric/AT&T Model 500 telephone
- Transformer-isolated two-way audio interface
- ESP32-S3 firmware for ringing, hookswitch handling, rotary dialing, call control, diagnostics, and dialing safety

## What v0.8 can do

- Ring the original mechanical bells for incoming cellular calls
- Answer when the handset is lifted
- End a call when the handset is replaced
- Decode rotary dial pulses
- Place outgoing cellular calls
- Pass two-way voice audio
- Report phone and modem state over USB serial
- Reject exact `911`, `900...`, `1900...`, and numbers shorter than seven digits before sending an `ATD` command

## Repository layout

```text
ESP-RotaryCell/
├── README.md
├── CHANGELOG.md
├── ROADMAP.md
├── v0.8/
│   ├── Firmware/
│   ├── Hardware/
│   ├── Documentation/
│   └── Mechanical/
└── Development/
    ├── Older_Firmware/
    └── Experimental_Hardware/
```

`v0.8` is the known-working baseline. Experimental work belongs under `Development` until it is validated for a later release.

## Start here

1. Read `v0.8/Documentation/Release_Notes.md`.
2. Review `v0.8/Hardware/ESP-CellCore_Service_Schematic_RevB.pdf`.
3. Read `v0.8/Documentation/Build_Notes.md` and `Known_Issues.md`.
4. Open the Arduino sketch under `v0.8/Firmware/ESP-RotaryCell_v0.8/`.

## Safety

This design is for a self-contained telephone only. Do not connect its TIP/RING pair to public telephone service, house telephone wiring, or any other external line. Use a protected single-cell Li-ion battery and observe normal Li-ion handling and charging precautions.
