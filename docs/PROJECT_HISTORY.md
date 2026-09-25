# Project history and engineering references

This page preserves the development context behind RotaryCell. Builders should
use the [illustrated build guide](https://evilroot.net/RotaryCell/) for the
current PCB construction process and [STATUS.md](../STATUS.md) for the latest
validation results.

## From hand-wired prototype to custom boards

The first working RotaryCell was assembled point-to-point before the dedicated
PCBs were available. It established the core design: a LilyGO
T-SIM7670G-S3 controller/modem, Silvertel AG1171 subscriber-line interface,
21700 battery, passive audio interface, and the original Western Electric 500
telephone hardware.

The prototype remains documented as an engineering reference:

[![RotaryCell hand-wired prototype schematic](images/complete-prototype-wiring.png)](RotaryCell_Complete_Prototype_Wiring.pdf)

[Open the full-resolution prototype wiring PDF](RotaryCell_Complete_Prototype_Wiring.pdf).

The later Audio/Reset A4 and AG1171 carrier boards replaced most of the loose
point-to-point assembly. These are the boards used by the illustrated guide.
Their as-ordered manufacturing files are preserved under
[`hardware/audio-reset-a4/`](../hardware/audio-reset-a4/) and
[`hardware/ag1171-carrier-through-hole/`](../hardware/ag1171-carrier-through-hole/).

## September 2026 PCB baseline

At the documented baseline:

- Firmware **v0.10.4** was the stable public release.
- Firmware **v0.11.1-dev** was the active hardware-validation build.
- Multiple Audio/Reset A4 and AG1171 carrier assemblies were operational.
- Incoming and outgoing calls, rotary dialing, ringing, handset audio,
  charging, AG1171 idle power saving, and manual hardware reset had passed
  testing on assembled PCB builds.
- Service code `9999` used the A4 hardware power-cycle circuit to recover an
  observed modem state in which incoming calls went directly to voicemail.
- The optional INA226/microSD logger produced useful current records, while its
  bus-voltage and calculated-power fields still required correction.

Current test results and remaining work are maintained in
[STATUS.md](../STATUS.md).

## System architecture

- The **LilyGO T-SIM7670G-S3 Standard** supplies the ESP32-S3 controller,
  A7670 cellular modem, charging system, and cellular audio interface.
- The **Silvertel AG1171** operates the telephone line circuitry, senses the
  switch-hook, and drives the mechanical ringer.
- The **Audio/Reset A4** board provides adjustable transmit and receive audio
  conditioning plus a hardware modem power-cycle path.
- A **21700 cell** connects through the LilyGO battery protection path. The
  AG1171 carrier takes `VPWR` from LilyGO `VBAT` and returns through system
  `GND`.

The telephone's repurposed line jack supplies regulated 5 V to the LilyGO
charging input on designated pins. It is not a telephone-line interface and
does not power the AG1171 directly.

More detail is available in [Architecture](ARCHITECTURE.md),
[hardware wiring](HARDWARE_WIRING.md), and the
[PCB connector pin guide](PCB_CONNECTOR_PIN_GUIDE.md).

## Firmware and service history

The firmware provides rotary pulse decoding, hook-state detection, call
control, bell ringing, locally generated telephone tones, battery monitoring,
maintenance Wi-Fi, browser and USB diagnostics, persistent event logging,
network time synchronization, and application updates.

Service code `0000` starts maintenance Wi-Fi. The USB command `WIFI ON` offers
access when the dial cannot be used. Service code `9999` records diagnostics
and triggers the A4 hardware power-cycle circuit in development firmware that
supports it.

Stable, development, and historical firmware are separated under `firmware/`
and `development/firmware/`. Toolchain instructions are in
[FIRMWARE_BUILDING.md](../FIRMWARE_BUILDING.md).

## Documentation archive

The top-level `docs/` directory retains architecture notes, bring-up records,
older BOM and wiring documents, and commissioning material. Versioned records
under `docs/legacy/` describe earlier firmware and hardware and may conflict
with the current PCB guide.

The repository keeps tested firmware, the exact board packages used for the
photographed builds, the current illustrated guide, and the engineering trail
that led to them. Later board revisions should be recorded separately rather
than replacing the archived as-ordered packages.
