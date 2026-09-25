# Troubleshooting reference

Use the [illustrated build guide](../guide/README.md) as the wiring authority.
When diagnosing a problem, identify every conductor by signal name and verify it
end-to-end with a meter; connector-shell orientation and wire color are not
reliable substitutes.

## Build does not power up

- Confirm cell polarity at the LilyGO battery-positive and `BATN` points.
- Keep cell-negative `BATN` separate from system GND. Only the cell-negative
  lead belongs on `BATN`.
- Verify the physical power switch and the A4 `RAW_BAT`/`SW_BAT` connections
  with continuity mode.
- Disconnect external USB while testing whether a battery-path reset actually
  removes power from the complete LilyGO board.

## Cellular modem does not register

- Attach the cellular antenna before allowing the modem to transmit.
- Confirm that the SIM is activated for cellular voice service rather than data
  alone.
- Allow roughly a minute for initialization and registration, then use the USB
  console `S` command to inspect modem, SIM, registration and operator state.

## No dial tone or telephone audio

- Verify the LilyGO-to-A4 connector as GPIO36 audio signal, GND, `SPEK+` and
  `MIC+`; an apparently normal connector can still be wired in reverse.
- Leave `SPEK-` floating and insulated.
- Connect `MIC-` to system GND only through the external 1 uF nonpolar
  capacitor.
- Begin with both A4 trimmers near halfway and adjust during a test call.
- Verify that A4 J2 to carrier U2 is straight through: pin 1 to 1, 2 to 2 and
  3 to 3.

## Rotary digits or hook state are wrong

- Confirm carrier U3 `SHK` reaches GPIO37.
- Watch the USB console while operating the switch-hook and dial.
- Check the original dial contacts and mechanism before changing firmware
  timing; old contacts may be dirty or mechanically slow.

## Incoming calls do not ring the bell

- Confirm the AG1171 has `VBAT` and LilyGO system GND at carrier CN1.
- Verify carrier U3 `FR` to GPIO15 and `RM` to GPIO16.
- Confirm the internal Tip/Ring harness reaches the telephone's original
  incoming-line points. Network-block terminal positions differ among phones.

## Hardware recovery does not restart the phone

Service code `9999` records diagnostics and triggers the A4 hardware power
cycle when using the required guide firmware. Test it with external USB power
disconnected; USB can keep the controller powered while the battery path is
interrupted. Verify GPIO35, `RAW_BAT` and `SW_BAT` against the guide before
changing reset timing.

## Recording a failure

Record the firmware version, board revisions, cellular provider, observed
behavior, relevant console output or logs, measurements and the change that
resolved the problem. This helps distinguish confirmed fixes from guesses.
