# Current hardware assembly guide

This guide covers assembly of the August 2026 board candidates and the confirmed inter-board harnesses. It intentionally stops where the present record lacks a physical terminal or mounting detail. Use [Current hardware wiring](HARDWARE_WIRING.md), [Master BOM](MASTER_BOM.md), and [First-board bring-up](BRINGUP.md) alongside it.

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
- Connect `MIC-` to common ground through a 1 uF film or bipolar capacitor located at the LilyGO/harness.

These connections follow the working prototype's RevA2 passive-audio reference. The four J1 conductors remain GPIO36, GND, `SPEK+`, and `MIC+`.

### LilyGO to carrier logic

Make one four-conductor cable:

- Carrier U3 pin 1 -> GPIO15 (`FR`)
- Carrier U3 pin 2 -> GPIO16 (`RM`)
- Carrier U3 pin 3 -> GPIO37 (`SHK`)
- Carrier U3 pin 4 -> leave disconnected until the AG1171 `PD` GPIO is assigned and tested

## 5. Initial powered assembly

1. Leave the AG1171 `PD` lead and the A4 reset harness disconnected.
2. Use the protected 21700 or a current-limited bench supply set to an appropriate single-cell Li-ion voltage. Connect carrier CN1 pin 1 to cell negative/common ground and pin 2 directly to cell positive/VPWR. Never apply the regulated 5 V charging input to CN1.
3. Verify cell voltage and polarity at AG1171 pins 12 and 13.
4. Connect the LilyGO/audio and audio/carrier harnesses.
5. Power the LilyGO and perform the staged tests in [COMMISSIONING.md](COMMISSIONING.md).

## 6. Telephone and charging connections - documentation required

Carrier CN2 provides Ring(B) on pin 1 and Tip(A) on pin 2, but this guide does not yet identify the exact Model 500 network-block screw terminals. Likewise, it does not establish the RJ11 pins that deliver regulated 5 V to the LilyGO charging input. Do not infer either connection from wire color or jack position.

Before publication as a reproducible build, add:

- A straight-on photograph of the network block with the two connection terminals labeled.
- Front and rear photographs of the RJ11 jack with pin numbering, regulated 5 V polarity, and protection shown.
- A close-up of the LilyGO power switch with `RAW_BAT` and `SW_BAT` solder points labeled.

## 7. Mechanical installation - provisional

The working arrangement places the LilyGO toward the front of the telephone, the 21700 between the internal sheet-metal posts, and the smaller boards to either side as clearance permits. A finalized snap-in LilyGO/21700 carrier is not yet part of the repository. Secure every assembly against movement, insulate it from the metal base and mechanism, preserve dial clearance, and keep the cellular antenna away from low-level audio wiring.

The installation is not complete until the mechanical carrier and cable strain relief are documented and validated with the telephone housing closed.
