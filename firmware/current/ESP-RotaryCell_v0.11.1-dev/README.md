# RotaryCell v0.11.1-dev

This is the current supported firmware for the illustrated Audio/Reset A4 and
AG1171 carrier-board build. It remains development-labelled while endurance
testing and automatic modem-recovery work continue. Stable release v0.10.4 is
retained under `firmware/archive` for older installations.

## Additions over v0.10.4

- Service code `9999` records a pre-reset diagnostic snapshot and triggers the
  Audio/Reset A4 battery-path power cycle through GPIO35.
- A matching post-reset snapshot is saved after reboot, including raw `AT+CGMR`
  and `AT+CIREG?` responses.
- Experimental AG1171 idle power saving can be enabled from the maintenance
  page. GPIO21 only pulls `PD` low or releases it to high impedance.
- Optional INA226 and microSD power logging records 10 Hz current samples in
  `POWER###.CSV` files.
- Persistent events and microSD power logs can be downloaded from the
  maintenance page.
- `WIFI ON` or `WIFI` starts maintenance Wi-Fi from the USB console.
- The maintenance page can suspend and restore the ten-minute Wi-Fi countdown.
- A successful web firmware update requests maintenance Wi-Fi once after reboot
  so the updated phone can be reached without dialing `0000`.

## Current validation

Incoming and outgoing calling, dialing, ringing, audio, AG1171 power saving,
the battery-powered `9999` reset path, paired reset diagnostics, and physical
log downloads have passed testing on assembled hardware. The hardware reset
cleared an observed modem condition in which incoming calls went directly to
voicemail.

Automatic detection of that modem failure is not implemented because a
dependable failure signature has not yet been identified. The optional INA226
logger currently produces useful current measurements, but its bus-voltage and
calculated-power fields remain zero.

The checked-in configuration has `ENABLE_DEVELOPMENT_POWER_LOGGING` enabled for
the current instrumented test phone. Set it to `false` before building for a
phone without the INA226 and microSD hardware.

See [RESET_DIAGNOSTICS.md](RESET_DIAGNOSTICS.md) and
[MAINTENANCE_DOWNLOADS.md](MAINTENANCE_DOWNLOADS.md) for detailed behavior.
