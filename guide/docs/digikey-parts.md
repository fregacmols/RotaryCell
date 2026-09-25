<span class="kicker">RotaryCell / build guide</span>

# Through-hole parts

This is the per-phone shopping list for the hand-populated AG1171 carrier and
the external capacitor used in the audio connection. Prices are Digi-Key
single-quantity prices checked September 24, 2026. Shipping, sales tax and any
tariffs are additional.

## AG1171 carrier parts

| Part | Qty. | Reference | Source | Extended price |
| --- | ---: | --- | --- | ---: |
| Silvertel AG1171 module | 1 | U1 | [5061-AG1171-ND](https://www.digikey.com/en/products/detail/silvertel/AG1171/21187236) | **$8.50** |
| 1 kΩ, ¼ W axial resistor | 3 | R1–R3 | [1.0KQBK-ND](https://www.digikey.com/en/products/detail/yageo/CFR-25JB-52-1K/96) | **$0.30** |
| 470 µF, 16 V radial electrolytic | 1 | C1 | [493-1785-ND](https://www.digikey.com/en/products/detail/nichicon/UPW1C471MPD/589526) | **$0.62** |
| 100 nF, 50 V radial ceramic | 1 | U4 | [BC2665CT-ND](https://www.digikey.com/en/products/detail/vishay-beyschlag-draloric-bc-components/K104K10X7RF5UH5/2356754) | **$0.30** |
| 1N4148 DO-35 diode | 1 | D2 | [1N4148FS-ND](https://www.digikey.com/en/products/detail/onsemi/1N4148/458603) | **$0.10** |
| JST XH 2-pin vertical header | 2 | CN1, CN2 | [B2B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B2B-XH-A/1651045) | **$0.20** |
| JST XH 3-pin vertical header | 1 | U2 | [B3B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B3B-XH-A/1651046) | **$0.13** |
| JST XH 4-pin vertical header | 1 | U3 | [B4B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B4B-XH-A/1651047) | **$0.17** |
|  |  |  | **Parts total** | **$10.32** |

The board labels the 100 nF capacitor as `U4`; that unusual reference is
intentional. Select **cut tape** for `BC2665CT-ND`. The specified diode is a
1N4148 even though the working prototype used a BAT85 that was already on hand.

If your connector assortment already supplies the four PCB headers, omit the
last three lines. The remaining carrier parts total **$9.82**.

## External audio capacitor

| Part | Qty. | Used at | Source | Extended price |
| --- | ---: | --- | --- | ---: |
| 1 µF, 50 V X7R radial ceramic capacitor | 1 | Between `MIC-` and system ground | [Murata RDER71H105K2M1H03A / 490-9146-1-ND](https://www.digikey.com/en/products/detail/murata-electronics/RDER71H105K2M1H03A/4771301) | **$0.69** |

The capacitor is nonpolar and compact enough to fit comfortably in the wiring
harness. The carrier parts plus this capacitor total **$11.01**, or **$10.51**
when the connector assortment supplies the carrier headers.

The [harness section](harnesses.md) covers the connector housings, crimp
contacts and wire.

## Optional assortments used in the prototype

I built the first carrier from component assortments already on my bench. You
do not need to buy these packs for one phone; the individual parts above are
the clearer shopping list.

| Existing assortment | Part taken from it |
| --- | --- |
| [EEEEE ceramic capacitor assortment](https://www.amazon.com/dp/B094HRK8QX?th=1) | One 100 nF ceramic capacitor |
| [BOJACK ¼ W resistor assortment](https://www.amazon.com/dp/B08FD1XVL6?th=1) | Three 1 kΩ resistors |
| [Swpeet electrolytic capacitor assortment](https://www.amazon.com/dp/B07KC99W2K) | One 470 µF electrolytic capacitor |
| [BAT85 diode pack](https://www.amazon.com/dp/B0CKSNPVH8?th=1) | One BAT85 used in the prototype instead of the specified 1N4148 |

## Reference only: Audio/Reset A4 through-hole parts

Do not use this section as a separate shopping list. Order the Audio/Reset A4
board fully assembled as part of the combined [PCB order](ordering.md). This
table is retained only as a reference for the through-hole positions on that
board.

| Part | Qty. | References | Source reference |
| --- | ---: | --- | --- |
| 10 kΩ through-hole trimmer | 2 | SPK_LVL, MIC_LVL | [Bourns 3362P-1-103LF](https://www.digikey.com/en/products/detail/bourns-inc/3362P-1-103LF/1088412) |
| JST XH 3-pin vertical header | 2 | J2, U2 | [B3B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B3B-XH-A/1651046) |
| JST XH 4-pin vertical header | 1 | J1 | [B4B-XH-A](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B4B-XH-A/1651047) |

The fully assembled Audio/Reset board should arrive with these parts and all
of its surface-mount components already installed. Confirm their placement in
the JLCPCB preview before ordering.
