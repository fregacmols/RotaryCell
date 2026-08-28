# Hardware files

The hardware archive is divided by confidence and purpose. The two August 2026 board directories are immutable records of the exact packages ordered from JLCPCB; they are not yet validated production designs.

## Ordered board candidates

### Audio and Reset A4

[`audio-reset-a4/`](audio-reset-a4/) contains the native EasyEDA schematic and PCB sources, Gerber/drill archive, JLCPCB BOM, and pick-and-place file submitted on August 27, 2026.

The board combines the prototype-tested passive audio network, two adjustable 10 kOhm level trimmers, and a new hardware power-cycle circuit built around an SN74LVC1G123 one-shot and DMP1012USS-13 P-channel MOSFET. The ordered timing components are 820 kOhm and 4.7 uF. Treat the schematic as authoritative for connector pin numbering.

### AG1171 carrier, through-hole

[`ag1171-carrier-through-hole/`](ag1171-carrier-through-hole/) contains the native EasyEDA schematic and PCB sources and Gerber/drill archive submitted on August 28, 2026.

This carrier is intended for hand assembly, so the order package has no assembly BOM or pick-and-place file.

## Other hardware material

- `prototype/` contains the Passive Audio Board RevA2 reference drawing associated with the working hand-wired prototype. An editable RevA2 source is not archived here.
- `experimental/` contains unfinalized alternatives, intermediate exports, footprint/symbol work, the abandoned USB-interlock concept, and earlier carrier iterations.
- `legacy/` contains older v0.8-era wiring and service documents.

Files in `experimental/` and `legacy/` may conflict with the current build and must not be treated as fabrication-ready.

## Revision and safety policy

Do not overwrite an ordered package when correcting a design. Add a new revision directory and record its relationship to the physical boards.

Review connectivity, component values, footprints, power switching, charging protection, grounding, and clearances before manufacturing or energizing anything. Never connect the prototype telephone-line interface or a repurposed charging jack to the public telephone network or energized premises telephone wiring.
