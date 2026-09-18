# Through-hole parts

I bought the AG1171 from Digi-Key using the link below. The rest of this list gives matching Digi-Key parts for the carrier. Quantities are for **one phone**.

The [crimp tool and connector kit](https://www.amazon.com/dp/B0C8N77PFF) I use includes the through-hole headers. The Digi-Key header links below are alternatives if you’re buying connectors separately.

## Assortments I used

I populated the carrier using parts from assortments and packs I already had on hand:

| Pack | Parts for this build |
| --- | --- |
| [EEEEE ceramic capacitor assortment](https://www.amazon.com/dp/B094HRK8QX?th=1) | 100 nF (0.1 µF) capacitor |
| [BOJACK ¼ W resistor assortment](https://www.amazon.com/dp/B08FD1XVL6?th=1) | Three 1 kΩ resistors |
| [Swpeet electrolytic capacitor assortment](https://www.amazon.com/dp/B07KC99W2K) | 470 µF electrolytic capacitor |
| [BAT85 diode pack](https://www.amazon.com/dp/B0CKSNPVH8?th=1) | I used a BAT85 for D2 because I had these on hand; the board specifies 1N4148 |

You don’t need to buy entire assortments for one phone. The individual Digi-Key options below are another way to get the parts.

## Individual parts from Digi-Key

| Qty | Reference | Part to order | Description / fit |
| ---: | --- | --- | --- |
| 1 | U1 | [Silvertel AG1171](https://www.digikey.com/en/products/detail/silvertel/AG1171/21187236) — 5061-AG1171-ND | The module I ordered |
| 3 | R1–R3 | [YAGEO CFR-25JB-52-1K](https://www.digikey.com/en/products/detail/yageo/CFR-25JB-52-1K/96) — 1.0KQBK-ND | 1 kΩ, ¼ W, axial resistor |
| 1 | C1 | [Nichicon UPW1C471MPD](https://www.digikey.com/en/products/detail/nichicon/UPW1C471MPD/589526) — 493-1785-ND | 470 µF, 16 V radial electrolytic; 5 mm lead spacing |
| 1 | U4 | [Vishay K104K10X7RF5UH5](https://www.digikey.com/en/products/detail/vishay-beyschlag-draloric-bc-components/K104K10X7RF5UH5/2356754) — BC2665CT-ND | 100 nF (0.1 µF) ceramic capacitor; marked U4 on the board |
| 1 | D2 | [onsemi 1N4148](https://www.digikey.com/en/products/detail/onsemi/1N4148/458603) — 1N4148FS-ND | 1N4148, DO-35 axial diode — specified in the design and on the silkscreen |
| 2 | CN1, CN2 | [JST B2B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B2B-XH-A/1651045) | 2-pin vertical XH PCB header, 2.50 mm pitch |
| 1 | U2 | [JST B3B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B3B-XH-A/1651046) | 3-pin vertical XH PCB header, 2.50 mm pitch |
| 1 | U3 | [JST B4B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B4B-XH-A/1651047) | 4-pin vertical XH PCB header, 2.50 mm pitch |

Choose **cut tape** for the ceramic capacitor; you only need one. For the resistors, order three, or ten if you’d like a few spares.

## Audio/Reset A4: buy only if omitted from PCB assembly

I designed the Audio/Reset PCB to be **ordered assembled from JLCPCB**. It works out cheaper than buying the components separately and installing them yourself. Factory assembly also lets me use much smaller passive components than would be comfortable to solder by hand.

The parts below are only for any through-hole positions left unpopulated by the assembly order; they are not a shopping list for hand-building the audio board.

| Qty if unpopulated | References | Part |
| ---: | --- | --- |
| 2 | SPK_LVL, MIC_LVL | [Bourns 3362P-1-103LF](https://www.digikey.com/en/products/detail/bourns-inc/3362P-1-103LF/1088412), 10 kΩ through-hole trimmer |
| 2 | J2, U2 | [JST B3B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B3B-XH-A/1651046), 3-pin vertical header |
| 1 | J1 | [JST B4B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B4B-XH-A/1651047), 4-pin vertical header |

If both boards need all their headers fitted, the combined header quantities are **two 2-pin, three 3-pin, and two 4-pin**. Do not buy the audio board's surface-mount resistors, capacitors, or ICs again when those are supplied by JLCPCB.

## Wiring supplies

The [harness section](HARDWARE_WIRING.md) will cover housings, crimp contacts, wire, and the external 1 µF microphone-reference capacitor.
