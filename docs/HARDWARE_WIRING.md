# Current hardware wiring and connector pinouts

**Baseline:** Audio and Reset A4 plus AG1171 Carrier Through-Hole, as ordered August 2026

This document translates the ordered EasyEDA schematics into cable-level connections. The native EasyEDA sources remain authoritative. Pin numbers below are electrical pin numbers; do not infer pin 1 from a left-to-right view of a loose mating housing. Confirm the pin-1 mark on the PCB and header before crimping a harness.

## System interconnect

```mermaid
flowchart LR
    L[LilyGO T-A7670G-S3] -->|GPIO36, GND, SPEK+, MIC+| A[Audio and Reset A4]
    A -->|VIN, GND, VOUT| C[AG1171 carrier]
    L -->|GPIO15 FR, GPIO16 RM, GPIO37 SHK, PD control TBD| C
    P[Regulated AG1171 supply] -->|VPWR, GND| C
    C -->|Tip and Ring| N[Telephone network block]
    B[Protected 21700 cell] --> L
    A -.->|RAW_BAT, SW_BAT, reset trigger| L
```

## Audio and Reset A4

### J1 - LILYGO, 4-pin JST-XH

| J1 pin | Board signal | Connect to |
| ---: | --- | --- |
| 1 | Tone/PWM input | LilyGO ESP32-S3 GPIO36 |
| 2 | Common GND | LilyGO GND |
| 3 | Receive audio input | LilyGO `SPEK+` |
| 4 | Transmit audio output | LilyGO `MIC+` |

The ordered board uses one signal conductor in each audio direction. The screenshot does not establish the final LilyGO-side treatment of `SPEK-` and `MIC-`; record that connection from the working harness before calling this a complete external wiring specification. Do not assume an audio-negative terminal may be directly grounded merely because J1 includes common ground.

### J2 - AG1171_AUDIO, 3-pin JST-XH

| J2 pin | Board signal | Carrier connection |
| ---: | --- | --- |
| 1 | AG1171 `VIN` | Carrier U2 pin 1 |
| 2 | Common GND | Carrier U2 pin 2 |
| 3 | AG1171 `VOUT` | Carrier U2 pin 3 |

This cable is straight-through: 1-to-1, 2-to-2, and 3-to-3.

### U2 - hardware power-cycle, 3-pin JST-XH

| U2 pin | Schematic net | Intended connection |
| ---: | --- | --- |
| 1 | `SW_BAT` | Switched side of the LilyGO physical power-switch path |
| 2 | `RAW_BAT` | Raw cell-positive side of that switch path |
| 3 | Trigger | Future LilyGO reset-control GPIO |

The reset circuit shares ground through J1 pin 2. Firmware v0.10.4 does not assign U2 pin 3. Keep this harness unplugged until the one-shot and switch nodes have been verified with a meter. The schematic identifies the electrical nodes, but the exact LilyGO switch solder points still require a close-up photograph or pad-level drawing.

### Audio paths

- Receive: `SPEK+` -> C1 1 uF -> R1 10 kOhm -> SPK_LVL 10 kOhm -> C2 10 nF -> AG1171 `VIN`.
- Tone injection: GPIO36 -> R2 3.3 kOhm with C3 22 nF to ground -> C4 220 nF -> R3 1 kOhm -> receive summing node.
- Transmit: AG1171 `VOUT` -> C5 100 nF -> MIC_LVL 10 kOhm -> R4 10 kOhm -> `MIC+`.

![Audio portion of the Audio and Reset A4 schematic](images/audio-a4-audio-schematic.png)

The image above covers the audio portion only. The reset circuit is present in the native A4 EasyEDA source.

## AG1171 Carrier Through-Hole

### CN1 - AG1171 power, 2-pin JST-XH

| CN1 pin | Signal | Connection |
| ---: | --- | --- |
| 1 | `GND PWR` | Supply ground / common ground |
| 2 | `VPWR` | Regulated AG1171 supply |

The present design expects regulated 5 V for the AG1171 supply. Confirm voltage and polarity at the empty socket before installing the AG1171.

### U2 - audio, 3-pin JST-XH

| U2 pin | AG1171 signal | Audio-board connection |
| ---: | --- | --- |
| 1 | `VIN`, AG1171 pin 9 | Audio J2 pin 1 |
| 2 | Common GND | Audio J2 pin 2 |
| 3 | `VOUT`, AG1171 pin 10 | Audio J2 pin 3 |

### U3 - logic, 4-pin JST-XH

| U3 pin | Carrier function | LilyGO connection |
| ---: | --- | --- |
| 1 | `FR`, through R3 1 kOhm | GPIO15 |
| 2 | `RM`, through R2 1 kOhm | GPIO16 |
| 3 | `SHK`, through R1 1 kOhm | GPIO37 |
| 4 | `PD`, through D2 1N4148 | Power-down control GPIO not assigned in v0.10.4 |

Do not connect U3 pin 4 to an arbitrary GPIO until its active level, boot behavior, and firmware assignment have been documented and tested.

### CN2 - telephone line, 2-pin JST-XH

| CN2 pin | AG1171 signal | Intended connection |
| ---: | --- | --- |
| 1 | `Ring(B)`, AG1171 pin 1 | Telephone's internal line/network-block connection |
| 2 | `Tip(A)`, AG1171 pin 2 | Telephone's internal line/network-block connection |

The exact Model 500 network-block screw terminals are not established by these screenshots. Add a telephone-specific close-up and terminal designation before presenting this as a complete assembly instruction. This internal Tip/Ring pair must never be connected to the PSTN or energized premises wiring.

![AG1171 Carrier Through-Hole schematic](images/ag1171-carrier-through-hole-schematic.png)

## AG1171 pins used by the carrier

| AG1171 pin | Name | Routed to |
| ---: | --- | --- |
| 1 | Ring(B) | CN2 pin 1 |
| 2 | Tip(A) | CN2 pin 2 |
| 3 | FR | U3 pin 1 through 1 kOhm |
| 4 | RM | U3 pin 2 through 1 kOhm |
| 5 | SHK | U3 pin 3 through 1 kOhm |
| 6, 7, 8 | NC | No connection |
| 9 | VIN | U2 pin 1 |
| 10 | VOUT | U2 pin 3 |
| 11 | NC | No connection |
| 12 | GND PWR | CN1 pin 1 and U2 pin 2 |
| 13 | VPWR | CN1 pin 2 |
| 14 | PD | U3 pin 4 through D2 |

## Connections still requiring physical documentation

1. LilyGO-side `SPEK-` and `MIC-` treatment at the audio connectors.
2. Exact solder pads on the LilyGO physical power switch for `RAW_BAT` and `SW_BAT`.
3. Exact telephone network-block terminals for CN2 Tip and Ring.
4. RJ11 charging pin numbers, polarity, fuse/protection, and regulated 5 V source.
5. Final reset-trigger and AG1171-PD GPIO assignments.

Until these five items are recorded, this is a PCB interconnect specification rather than a complete telephone wiring diagram.
