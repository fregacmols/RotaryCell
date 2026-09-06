# PCB connector pin guide

**Build baseline:** firmware v0.10.4, Audio and Reset A4, and AG1171 Carrier Through-Hole.

## How to identify the installed JST pins

All orders below are shown looking straight down at the **component side** of each PCB. Orient the board using the stated landmarks; the connector labels and pin-1 marks may be hidden after assembly.

### Audio and Reset A4

Hold the board with the two blue level trimmers at the top and the `RotaryCell` text along the bottom edge.

```text
              SPK trimmer                    MIC trimmer

 J1, top ──>  [4 MIC+]                  [1 VIN]  <── J2, top
              [3 SPEK+]                 [2 GND]
              [2 GND]                   [3 VOUT]
 J1, bottom > [1 GPIO36]

                         U2 [3 RESET] [2 RAW_BAT] [1 SW_BAT]
                                  left  ─────────────>  right
```

### AG1171 carrier

Hold the board with the connector area at the top and the long AG1171 module/header section along the bottom.

```text
 AUDIO U2  [1 VIN] [2 GND] [3 VOUT]       PWR CN1  [2 VPWR] [1 GND]
             left  ─────────>  right                   left ──> right

 LILYGO U3 [4 PD] [3 SHK] [2 RM] [1 FR]   NETWORK   [2 TIP] [1 RING]
             left  ─────────>  right         CN2       left ──> right

                         AG1171 module/header section
```

These are board-side views. A loose female cable housing can appear reversed when it is turned around. Build cables by electrical pin number and confirm every conductor end-to-end with a continuity meter; do not rely on wire color or the apparent left-to-right order of an unplugged housing.

## LilyGO correlation table

| LilyGO connection | PCB connection | Function |
| --- | --- | --- |
| GPIO36 | Audio A4 J1 pin 1 | Dial-tone PWM |
| GND | Audio A4 J1 pin 2 | Common ground |
| `SPEK+` | Audio A4 J1 pin 3 | Receive audio from LilyGO |
| `MIC+` | Audio A4 J1 pin 4 | Transmit audio to LilyGO |
| `SPEK−` | No connection | Leave floating and insulate |
| `MIC−` | 1 uF film/bipolar capacitor, then GND | External AC ground reference |
| GPIO15 | Carrier U3 pin 1 | AG1171 `FR` |
| GPIO16 | Carrier U3 pin 2 | AG1171 `RM` |
| GPIO37 | Carrier U3 pin 3 | AG1171 `SHK` |
| GPIO21 | Carrier U3 pin 4 | AG1171 `PD` through D2/BAT85; high-impedance normally and LOW for power-down; never drive HIGH |
| `VBAT` header pad | Carrier CN1 pin 2 | AG1171 `VPWR` |
| System GND | Carrier CN1 pin 1 | AG1171 power return; do not connect to holder negative/`BATN` |
| LilyGO SW3 board-side pad | Audio A4 U2 pin 1 | `SW_BAT`; exact physical pad still to be confirmed |
| LilyGO SW3 battery-side pad | Audio A4 U2 pin 2 | `RAW_BAT`; exact physical pad still to be confirmed |
| GPIO35 | Audio A4 U2 pin 3 | Rising-edge reset trigger; leave harness unplugged until the reset board is bench-tested |

GPIO35 is adjacent to the GPIO36 tone pad. GPIO15, GPIO16, GPIO37, and GPIO21 are four consecutive pads on the opposite LilyGO header, keeping the Audio/Reset and AG1171 harnesses physically grouped.

## Audio-board to carrier cable

This is a straight-through three-wire cable:

- Audio A4 J2 pin 1 → Carrier U2 pin 1 (`VIN`)
- Audio A4 J2 pin 2 → Carrier U2 pin 2 (GND)
- Audio A4 J2 pin 3 → Carrier U2 pin 3 (`VOUT`)

Connector names repeat between boards: **A4 U2 is the reset connector; Carrier U2 is the audio connector.** Use electrical pin numbers and the PCB pin-1 marks, not the apparent left-to-right order of a loose JST housing.
