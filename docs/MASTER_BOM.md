# Master bill of materials

**Baseline:** August 28, 2026

This BOM combines the major system items with the two ordered board assemblies. Quantities are for one telephone. Items marked **installation-specific** must be selected after the final harness and mechanical design are documented.

## Major assemblies

| Qty | Item | Current prototype / requirement | Status |
| ---: | --- | --- | --- |
| 1 | Rotary telephone | Western Electric/AT&T Model 500-type set with functional dial, hookswitch, network block, handset, and ringer | Existing phone; not modified |
| 1 | Controller/modem | LilyGO T-A7670G-S3 Standard with A7670G modem | Prototype-tested |
| 1 | Cellular antenna | Compatible antenna and feed for the LilyGO modem | Normally supplied with board |
| 1 | SIM and service | Voice-capable SIM/service compatible with the modem and local network | Installation-specific |
| 1 | Subscriber-line interface | Silvertel AG1171-S | Carrier-mounted |
| 1 | Cell | Protected single 21700 Li-ion cell; Samsung 58E used in the prototype | Directly feeds LilyGO battery pads and carrier CN1 |
| 1 | Audio/reset PCB | Audio and Reset A4 | Ordered; awaiting validation |
| 1 | Carrier PCB | AG1171 Carrier Through-Hole | Ordered; awaiting validation |
| 1 | Charging source | Regulated 5 V input through the RJ11 to the LilyGO charging input | Does not directly power carrier CN1 |

## Audio and Reset A4 assembly

The exact JLCPCB assembly list, including manufacturer and LCSC part numbers, is preserved as [`hardware/audio-reset-a4/BOM_ESP-CellCore-Passive-Audio-Board-Rev-A4-Reset-VALIDATED_2026-08-27.csv`](../hardware/audio-reset-a4/BOM_ESP-CellCore-Passive-Audio-Board-Rev-A4-Reset-VALIDATED_2026-08-27.csv).

Summary:

| Qty | References | Value / part | Package |
| ---: | --- | --- | --- |
| 1 | C1 | 1 uF | 0603 |
| 1 | C2 | 10 nF | 0603 |
| 1 | C3 | 22 nF | 0603 |
| 1 | C4 | 220 nF | 0603 |
| 2 | C5, C7 | 100 nF | 0603 |
| 1 | C6 | 4.7 uF | 0805 |
| 1 | J1 | JST B4B-XH-A, 4-pin | Through-hole |
| 1 | J2 | JST B3B-XH-A, 3-pin | Through-hole |
| 1 | U2 | JST B3B-XH-A, 3-pin | Through-hole |
| 1 | Q1 | DMP1012USS-13 P-channel MOSFET | SO-8 |
| 2 | R1, R4 | 10 kOhm | 0603 |
| 1 | R2 | 3.3 kOhm | 0603 |
| 1 | R3 | 1 kOhm | 0603 |
| 1 | R5 | 820 kOhm | 0603 |
| 1 | R6 | 10 kOhm | 0603 |
| 2 | R7, R9 | 100 kOhm | 0603 |
| 1 | R8 | 1 kOhm | 0603 |
| 1 | U1 | SN74LVC1G123DCTT one-shot | SM8 |
| 2 | SPK_LVL, MIC_LVL | Bourns 3362P-1-103LF, 10 kOhm | Through-hole trimmer |

## AG1171 Carrier Through-Hole assembly

| Qty | Reference | Value / part | Notes |
| ---: | --- | --- | --- |
| 1 | U1 | Silvertel AG1171-S | Observe pin-1 orientation |
| 1 | C1 | Nichicon UPW1C471MPD, 470 uF 16 V | Polarized electrolytic |
| 1 | U4 | Vishay K104K10X7RF5UH5, 100 nF | Capacitor despite `U4` designator |
| 1 | D2 | 1N4148 | DO-35; observe polarity |
| 3 | R1-R3 | 1 kOhm | Axial DIN0207, 10.16 mm pitch |
| 2 | CN1, CN2 | JST B2B-XH-A, 2-pin | Through-hole vertical header |
| 1 | U2 | JST B3B-XH-A, 3-pin | Audio header |
| 1 | U3 | JST B4B-XH-A, 4-pin | Logic header |

## Harnesses and installation materials

| Qty | Item | Use |
| ---: | --- | --- |
| 2 | JST XHP-4 housing plus crimp contacts | LilyGO/audio harness and LilyGO/carrier logic harness |
| 3 | JST XHP-3 housing plus crimp contacts | Both ends of audio-to-carrier cable plus reset harness |
| 2 | JST XHP-2 housing plus crimp contacts | Carrier power and internal Tip/Ring harnesses |
| 1 | 1 uF film or bipolar capacitor | AC-reference LilyGO `MIC-` to common ground at the LilyGO/harness |
| As required | Stranded hookup wire, heat-shrink, sleeving, strain relief | Harness construction |
| As required | Fuse/protection, insulated battery harness, connectors | Final battery and charging design |
| As required | Nonconductive mounting hardware or printed carrier | Mechanical installation; design pending |

Housing counts assume the PCB headers listed above and a straight-through J2-to-U2 audio cable. Verify actual pre-crimped harness availability and pin order before purchase.

The protected 21700 positive lead branches to both the LilyGO battery pads and carrier CN1 pin 2. Cell negative/common ground connects to the LilyGO and carrier CN1 pin 1. The regulated 5 V RJ11 input is a separate charging path into the LilyGO.

The external 1 uF `MIC-` reference capacitor corresponds to C6 in the RevA2 prototype audio drawing. Do not confuse it with C6 on the A4 PCB BOM, which is a 4.7 uF reset-timing capacitor.

## Not yet purchase-ready

The following still require an exact part number or documented implementation before this becomes a complete procurement BOM:

- The final protected 21700 cell and any inline fuse/connector.
- The regulated 5 V charging source and RJ11 charging harness.
- The LilyGO power-switch/reset harness attachment method.
- The final printed mechanical carrier and its hardware.
