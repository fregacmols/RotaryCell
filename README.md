# RotaryCell

RotaryCell converts a traditional rotary telephone into a self-contained,
battery-powered cellular phone while preserving the original dial, handset,
ringer, switch-hook, wiring, and case. The conversion is reversible and does
not require drilling or cutting the telephone.

![Completed red RotaryCell telephone on the workbench](guide/docs/images/rotarycell-featured-finished.png)

## Build one

**[Open the illustrated RotaryCell build guide](https://evilroot.net/RotaryCell/)**

The guide covers parts, PCB ordering, printed mounts, LilyGO preparation,
wiring, firmware, installation, audio adjustment, and final testing. It is the
recommended construction path for the current Audio/Reset A4 and AG1171
carrier boards.

[Watch the short introduction and demonstration](https://youtu.be/PO0PJNvnMdw)
· [Read the current project status](STATUS.md)

## What it does

- Places and receives cellular voice calls
- Reads the original rotary dial and switch-hook
- Rings the telephone's mechanical bell
- Uses the original handset for two-way audio
- Runs from a rechargeable 21700 cell
- Provides maintenance, diagnostics, event logging, and firmware updates
- Fits inside the telephone without permanent alteration

## Repository map

| Path | Contents |
| --- | --- |
| [`guide/`](guide/) | Source, photographs, diagrams, and downloads for the illustrated build guide |
| [`firmware/current/`](firmware/current/) | Stable firmware source |
| [`firmware/prebuilt/`](firmware/prebuilt/) | Prebuilt firmware and source archive |
| [`development/firmware/`](development/firmware/) | Active development firmware |
| [`hardware/`](hardware/) | PCB source, manufacturing files, printable hardware, and archived designs |
| [`docs/`](docs/) | Engineering notes, architecture, bring-up records, and project history |

The exact manufacturing packages used in the photographed builds are under
[`hardware/audio-reset-a4/`](hardware/audio-reset-a4/) and
[`hardware/ag1171-carrier-through-hole/`](hardware/ag1171-carrier-through-hole/).

## Safety

- Do not connect the telephone wiring or repurposed charging jack to the public
  telephone network or energized premises wiring.
- Verify charging voltage, polarity, and pin assignment before connecting it.
- Use appropriate protection, insulation, restraint, and handling practices for
  the lithium-ion cell.

For development history, the hand-wired prototype, validation details, and
older references, see [Project history and engineering references](docs/PROJECT_HISTORY.md).

## License

Code and original documentation are licensed under the [MIT License](LICENSE).
Third-party datasheets, vendor names, trademarks, and historical telephone
designs remain the property of their respective owners.
