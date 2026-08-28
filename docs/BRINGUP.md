# First-board bring-up

This checklist applies to the August 2026 Audio and Reset A4 and AG1171 through-hole carrier orders.

## Before assembly

- Compare board outlines, connector orientation, and silkscreen labels with the EasyEDA source.
- Inspect for fabrication damage, solder-mask defects, and incomplete plated holes.
- Confirm there are no shorts between battery positive, switched battery, 5 V, 3.3 V, and ground.
- Confirm the AG1171 footprint orientation against the physical module before soldering.
- Confirm the JST-XH connector keying and pin numbering against the schematics.

## AG1171 carrier

1. Assemble passive parts and connectors before installing the AG1171.
2. Verify continuity from every connector pin to the intended AG1171 socket/pad.
3. Verify isolation between the line interface and logic/power nets where the schematic requires it.
4. Apply current-limited power without the telephone connected.
5. Install the AG1171 only after the carrier passes continuity checks.

## Audio board

1. Inspect JLCPCB component placement and polarity/orientation.
2. Leave the LilyGO hardware-reset harness disconnected initially.
3. Verify the passive audio path with the two trimmers near midrange.
4. Confirm clean transmit and receive audio and adequate adjustment range.
5. Verify idle current and that the MOSFET remains in the normal-on state.

## Reset circuit

1. Test with the telephone and AG1171 disconnected.
2. Current-limit the battery supply.
3. Confirm the reset input's idle level before connecting an ESP32 GPIO.
4. Apply a controlled trigger pulse and measure the switched-power interruption.
5. Confirm the LilyGO restarts reliably after the one-shot interval.
6. Confirm the physical power switch remains usable as a manual bypass.
7. Only then add firmware-controlled triggering.

## Integrated validation

- Incoming call and bell cadence
- Handset answer and hang-up
- Outgoing seven- and ten-digit dialing
- Dial, reorder, and howler tones
- Bidirectional audio quality
- Maintenance Wi-Fi and persistent logging
- Charging through the RJ11 charging pins
- Battery monitoring
- Extended powered endurance test
- Forced hardware-reset recovery test

Record failures and rework against the exact PCB revision; do not silently change the as-ordered source package.
