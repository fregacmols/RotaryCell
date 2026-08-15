# Known Issues — v0.8

## Receive audio level

Cell-to-rotary audio is understandable but somewhat quiet with the current 600:600 receive transformer and VR1 at maximum.

## Audio feedback and placement sensitivity

The far-end cellular caller can hear feedback or noise whose character changes when the transformer perfboard is moved. Transformer coupling, board placement, wiring pickup, and loop gain are suspected. VR2 adjustment reduces the problem.

## No audible local dial tone

GPIO36 tone generation exists in firmware, but the tone-injection components are not installed in the v0.8 hardware.

## Battery insertion wake-up

After the battery is physically removed and reinstalled, the LilyGO board requires a brief USB connection before it will start from battery. Normal use of the board's power switch does not require this step.

## No low-battery audible warning

Battery voltage is reported in diagnostics, but v0.8 does not alert the user through the handset.

## Short-code dialing

Numbers shorter than seven digits are deliberately blocked. This prevents accidental partial calls but also prevents intentional use of short service codes.

## Prototype construction

The hardware is perfboard/prototype wiring rather than a finalized PCB. Connector placement, wire routing, and mounting are build-specific.
