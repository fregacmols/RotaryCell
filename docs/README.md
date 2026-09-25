# Engineering references and project history

The current, photographed construction instructions are in the
**[illustrated RotaryCell build guide](../guide/README.md)** and are published at
<https://evilroot.net/RotaryCell/>. Use that guide when building with the
Audio/Reset A4 and AG1171 Carrier Through-Hole boards.

This directory preserves engineering notes from several stages of the project.
Some documents describe the earlier hand-wired prototype or the state of the
PCB design before the photographed builds were completed. They are useful for
design reasoning and troubleshooting, but they do not replace the illustrated
guide.

## Current technical references

- [Project status](../STATUS.md) — tested behavior and remaining development work
- [Firmware build settings](../FIRMWARE_BUILDING.md) — toolchain and required guide firmware
- [Architecture](ARCHITECTURE.md) — functional partitioning and GPIO assignments
- [PCB connector pin guide](PCB_CONNECTOR_PIN_GUIDE.md) — board-side connector orientation and pin mapping
- [Hardware wiring](HARDWARE_WIRING.md) — electrical detail behind the guide

## Development records

- [Project history](PROJECT_HISTORY.md) — prototype evolution and archive guide
- [Master engineering BOM](MASTER_BOM.md) — detailed board and harness inventory
- [First-board bring-up](BRINGUP.md) — staged validation used on the first PCB assemblies
- [Commissioning record](COMMISSIONING.md) — engineering-level test procedure
- [Prototype assembly notes](ASSEMBLY_GUIDE.md) — superseded pre-guide construction notes
- [Complete prototype wiring schematic](RotaryCell_Complete_Prototype_Wiring.pdf) — hand-wired RevA2 reference

Material under `legacy/` is grouped by the firmware version it originally
described and should be treated as historical.
