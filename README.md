# RotaryCell

RotaryCell converts a traditional rotary telephone into a self-contained cellular telephone **without modifying the original telephone**.

The design continues to use the original handset, rotary dial, switch-hook, mechanical ringer, network block, and existing jacks. The added electronics mount reversibly inside the case; no original telephone parts need to be drilled, cut, or permanently altered.

This is a working engineering archive rather than a finished construction release. The hand-wired prototype operates, while the first integrated PCBs are currently awaiting assembly and validation.

## Current baseline

This repository records the project as it stood on **August 28, 2026**:

- Firmware **v0.10.4** is the current prototype-tested software.
- The **Audio and Reset A4** PCB was ordered from JLCPCB on August 27, 2026.
- The **AG1171 Carrier Through-Hole** PCB was ordered from JLCPCB on August 28, 2026.
- Both ordered PCB packages are archived exactly as submitted and have not yet been validated as assembled boards.

See [STATUS.md](STATUS.md) for the distinction between tested prototype behavior and hardware awaiting validation.

## System overview

- A LilyGO T-A7670G-S3 Standard board supplies the ESP32-S3 controller, A7670 cellular modem, battery charging, and cellular audio interface.
- A Silvertel AG1171 subscriber-line interface operates the telephone line circuitry, senses the switch-hook, and drives the mechanical ringer.
- The Audio and Reset A4 PCB provides adjustable transmit/receive audio conditioning and a hardware power-cycle circuit for recovery when software-only modem reset is insufficient.
- A single protected 21700 cell connects to the LilyGO battery pads through a harness in place of the original 18650 holder and directly supplies the AG1171 carrier VPWR input.

The telephone's RJ11 line jack is used only to deliver regulated 5 V to the LilyGO charging input on the designated pins. It does not power the AG1171 directly and is not used as a telephone-line interface.

## Repository layout

| Path | Contents |
| --- | --- |
| `firmware/current` | Current Arduino sketch and source files |
| `firmware/prebuilt` | Current application OTA binary and source ZIP |
| `firmware/archive` | Historical firmware snapshots |
| `hardware/audio-reset-a4` | Exact Audio and Reset A4 source and manufacturing package |
| `hardware/ag1171-carrier-through-hole` | Exact through-hole carrier source and Gerber package |
| `hardware/prototype` | Material associated with the working hand-wired prototype |
| `hardware/experimental` | Unfinalized schematics, layouts, libraries, and alternatives |
| `hardware/legacy` | Older hardware documentation retained for reference |
| `docs` | Architecture, bring-up, and historical documentation |
| `site` | Draft project-page copy for evilroot.net |

Start with [STATUS.md](STATUS.md), [current hardware wiring](docs/HARDWARE_WIRING.md), the [master BOM](docs/MASTER_BOM.md), the [assembly guide](docs/ASSEMBLY_GUIDE.md), and [BUILDING.md](BUILDING.md).

## Current functions

- Rotary pulse dialing and switch-hook detection
- Incoming and outgoing cellular calls
- Physical bell ringing through the AG1171
- North American dial, reorder, and receiver-off-hook warning tones
- Bidirectional handset audio with adjustable levels
- Battery monitoring
- USB diagnostics and a temporary maintenance Wi-Fi dashboard
- Browser/USB AT-command terminal and persistent event log
- Cellular-network clock synchronization and application OTA updates

Dial service code `0000` starts maintenance Wi-Fi. Service code `9999` performs modem diagnostics and software recovery, but it did not recover the field-observed modem lockup described in [STATUS.md](STATUS.md).

## Important cautions

- Never connect prototype Tip/Ring wiring or the repurposed charging jack to the public telephone network or energized premises telephone wiring.
- Clearly label the charging jack and verify its regulated voltage, polarity, pin assignment, and protection before use.
- Lithium-ion cells require suitable protection, charging, fusing, insulation, and mechanical restraint.
- The maintenance access point uses the development password `rotarycell`. Change `WIFI_AP_PASSWORD` in `Config.h` before use around untrusted people.
- The August 2026 PCB files are **as ordered**, not yet production-tested. Create a new revision rather than silently replacing an as-ordered package.

## Repository policy

For now, this repository is best kept private on GitHub. It is intended to provide a durable, reproducible engineering baseline without presenting the current board candidates as a finished public release.

## License

Code and original documentation in this repository are licensed under the [MIT License](LICENSE). Third-party datasheets, vendor names, trademarks, and historical telephone designs remain the property of their respective owners.
