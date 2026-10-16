# Development firmware

This directory contains development firmware and the field-used 0.12 baseline.
The illustrated guide firmware remains under `../firmware/current`.

- `ESP-RotaryCell_v0.12.0-dev`: working 0.12 baseline, with organized
  maintenance navigation, call filters/history, scheduled/manual DND,
  configurable maintenance Wi-Fi, and persistent power-saving controls.
  Reported in use for several days on two phones without notable problems as
  of October 16, 2026. Bug fixes and minor feature additions or changes will
  use 0.12.x revisions. Its README, OTA binary, build record, and regression
  checklist are included in the same directory.

- The active tested v0.11.1 development build is now the supported guide
  firmware under [`../firmware/current`](../firmware/current/). This directory
  retains earlier experiments and engineering checkpoints.
- `ESP-RotaryCell_v0.11.0-dev`: earlier combined hardware-control build,
  retained as development history.
- `ESP-RotaryCell_v0.10.4_INA226_SD_Dev`: earlier isolated logger experiment,
  retained only as development history. Use the v0.11.1 build under
  `firmware/current` for the current combined hardware.

The matching web-uploadable application binary and its build record are in
[`../firmware/prebuilt`](../firmware/prebuilt/).
