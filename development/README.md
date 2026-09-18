# Development firmware

These sketches are experimental tools and are not part of the supported public
RotaryCell firmware release.

- `ESP-RotaryCell_v0.11.1-dev`: active tested development build. It combines
  service-code 9999 hardware-reset triggering and paired diagnostics,
  runtime-toggleable experimental AG1171 power saving, optional INA226/microSD
  logging, maintenance log downloads, USB-console Wi-Fi startup, a maintenance
  Wi-Fi timer override, and automatic maintenance Wi-Fi startup after a
  successful web firmware update.
- `ESP-RotaryCell_v0.11.0-dev`: earlier combined hardware-control build,
  retained as development history.
- `ESP-RotaryCell_v0.10.4_INA226_SD_Dev`: earlier isolated logger experiment,
  retained only as development history. Use the v0.11.1 development build for
  the current combined hardware tests.

The matching web-uploadable application binary and its build record are in
[`prebuilt`](prebuilt/).
