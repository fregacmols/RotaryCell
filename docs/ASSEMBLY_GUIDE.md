# Superseded prototype assembly notes

> **Historical reference:** These notes predate the photographed construction
> guide and are retained to explain first-board bring-up. For a current build,
> follow the [illustrated RotaryCell build guide](../guide/README.md). Where the
> two differ, the illustrated guide is authoritative.

## 1. Prepare the telephone

1. Confirm that the rotary dial, hookswitch, handset, network block, and mechanical ringer operate normally.
2. Disconnect the set from all telephone service and premises wiring.
3. Photograph the original wiring before moving anything.
4. Do not drill, cut, or remove original telephone components.

## 2. Inspect the PCBs

1. Match each board to its as-ordered Gerber and EasyEDA source directory.
2. Inspect the A4 assembly for misplaced parts, solder bridges, and damaged JST headers.
3. Check the carrier for plating defects and confirm the AG1171 pin-1 marking.
4. Perform the unpowered continuity checks in [BRINGUP.md](BRINGUP.md).

## 3. Assemble the AG1171 carrier

1. Install R1-R3, then D2 with the marked end in the schematic orientation.
2. Install U4, the 100 nF capacitor. Its `U4` designator is intentional in the ordered source.
3. Install C1, observing electrolytic polarity.
4. Install CN1, CN2, U2, and U3 with their keyed openings matching the PCB outline/source.
5. Inspect and continuity-test the board before installing the AG1171.
6. Install the AG1171 only after confirming VPWR is not shorted to ground and its orientation is correct.

## 4. Make the inter-board harnesses

### Audio board to carrier

Make one three-conductor straight-through JST-XH cable:

- Audio J2 pin 1 -> Carrier U2 pin 1 (`VIN`)
- Audio J2 pin 2 -> Carrier U2 pin 2 (GND)
- Audio J2 pin 3 -> Carrier U2 pin 3 (`VOUT`)

### LilyGO to audio board

Make one four-conductor cable following J1 in [HARDWARE_WIRING.md](HARDWARE_WIRING.md). At the LilyGO audio connectors:

- Leave `SPEK-` unconnected and insulated. Do not ground it.
- Connect `MIC-` to common ground through a 1 uF nonpolar capacitor located at the LilyGO/harness. The documented build uses a radial ceramic part.

These connections follow the working prototype's RevA2 passive-audio reference. The four J1 conductors remain GPIO36, GND, `SPEK+`, and `MIC+`.

### LilyGO to carrier logic

Make one four-conductor cable:

- Carrier U3 pin 1 -> GPIO15 (`FR`)
- Carrier U3 pin 2 -> GPIO16 (`RM`)
- Carrier U3 pin 3 -> GPIO37 (`SHK`)
- Carrier U3 pin 4 -> GPIO21 (`PD`); firmware must leave it high-impedance normally and pull it LOW only for power-down

## 5. Initial powered assembly

1. Use the required guide firmware before connecting the AG1171 `PD` lead and A4 reset harness.
2. Connect the cell only to LilyGO battery positive and `BATN`. Connect carrier CN1 pin 1 to LilyGO system GND and pin 2 to LilyGO `VBAT`. Never connect carrier ground directly to `BATN`, and never apply the regulated 5 V charging input to CN1.
3. Verify cell voltage and polarity at AG1171 pins 12 and 13.
4. Connect the LilyGO/audio and audio/carrier harnesses.
5. Power the LilyGO and perform the staged tests in [COMMISSIONING.md](COMMISSIONING.md).

## 6. Telephone and charging connections

Carrier CN2 provides Ring(B) on pin 1 and Tip(A) on pin 2. Network-block
terminals vary, so trace the telephone's original incoming line or use its
schematic rather than copying a photograph from another phone. The current
build replaces the original rear connector with the printed USB-C insert and
uses only the breakout board's `V+` and GND pads. See the illustrated guide for
the photographs and connector options.

## 7. Historical mechanical arrangement

The current printable carriers, mounting photographs and closure checks are in
the illustrated guide. Secure every assembly against movement, insulate it
from the metal base and mechanism, preserve dial clearance and keep the
cellular antenna away from low-level audio wiring.
