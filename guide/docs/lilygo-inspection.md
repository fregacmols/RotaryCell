<span class="kicker">RotaryCell / build guide</span>

# Inspect the prepared controller

Inspect and test the LilyGO before plugging in the Audio/Reset board, AG1171
carrier or battery.

## Visual inspection

- Battery-positive and `BATN` leads are identified and insulated.
- `VBAT` and system GND are routed to the carrier power connector.
- `SPEK-` is insulated and remains floating.
- The `MIC-` reference capacitor is present and insulated.
- GPIO15, GPIO16, GPIO21, GPIO35, GPIO36 and GPIO37 match their labeled harness positions.
- The power-switch taps have strain relief and cannot touch neighboring pads.
- Both USB ports and both QWIIC connectors remain accessible.

## Meter checks

With no battery or USB cable connected:

1. Check for an unintended short between battery positive and `BATN`.
2. Verify continuity through each new lead from its LilyGO pad to the correct
   connector contact.
3. Confirm that system GND and the carrier ground lead are continuous.
4. Confirm that the carrier ground lead is not wired directly to the raw
   holder-negative/`BATN` connection.
5. Verify that adjacent GPIO and audio contacts are not bridged.

![Rear of the prepared LilyGO with all corrected harnesses attached and arranged](images/lilygo-prepared-rear-corrected.jpg)

Use this overview to compare the general wire routing before fitting the
carrier. Exact solder joints are shown on the connection page; use the numbered
meter checks above to verify the electrical connections rather than judging
continuity from a photograph.

Fit the controller into the printed carrier and confirm that it seats fully.
The battery, signal and reset connectors must remain outside the carrier edges,
and the harnesses must pass through the open channel without being pinched.

After the unpowered checks pass, power the LilyGO by USB without the other
boards attached and confirm that it starts normally. Battery, charging and A4
reset tests belong in the staged commissioning procedure after their respective
connections have been verified.

<div class="checkpoint"><strong>Checkpoint</strong><br>The LilyGO starts from USB, every added conductor matches its documented node, and no unpowered short is present. Continue to custom-board assembly.</div>
