<span class="kicker">RotaryCell / build guide</span>

# Assemble the AG1171 carrier

The carrier is a straightforward through-hole board. We are soldering
through-hole components by hand here, like Neanderthals. Install the
low-profile parts first, followed by the capacitor and connectors, and add the
AG1171 module last.

![Bare carrier with all builder-installed parts laid out](images/carrier-parts-layout.jpg)

For one carrier you need three 1 kΩ resistors, one 100 nF capacitor, one
1N4148 diode, one 470 µF 16 V electrolytic capacitor, four keyed headers and
the AG1171 module.

## 1. Install the small parts

Fit the three resistors, the 100 nF capacitor and the diode. The resistors and
small capacitor can face either direction. Match the band on the 1N4148 to the
bar printed on its PCB outline.

![Resistors and diode installed before the larger components](images/carrier-small-parts-installed.jpg)

Solder the leads from the back and trim them close to the joints.

![Solder side after the small component leads have been trimmed](images/carrier-small-parts-soldered.jpg)

## 2. Add the capacitor and connectors

Install the 470 µF capacitor with its positive lead in the hole marked `+`.
Fit the four keyed headers in the orientations shown below: `LILYGO`, `AUDIO`,
`NETWORK` and `PWR`.

![Carrier populated with the capacitor, diode, resistors and four connectors](images/carrier-populated-before-ag1171.jpg)

## 3. Install the AG1171

Insert the AG1171 module through the long row of holes and solder every pin.
The module stands perpendicular to the carrier board.

![AG1171 module installed on the completed carrier](images/carrier-ag1171-installed.jpg)

The completed carrier should match the board at the top of this comparison.

![Completed AG1171 carrier beside a bare carrier PCB](images/carrier-complete-vs-bare.jpg)

Before moving on, look over the solder side for missed pins or bridges and
confirm that the diode and electrolytic capacitor match their printed polarity
marks.
