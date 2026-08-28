# Project status

**Baseline date:** August 28, 2026

**Firmware:** v0.10.4
**Hardware phase:** First integrated PCB order in fabrication

## Tested on the hand-wired prototype

- Original rotary dial pulse decoding
- Switch-hook detection
- Dial tone generation
- Incoming-call detection and physical bell ringing
- Answer and hang-up behavior
- Outgoing cellular calls
- Bidirectional handset audio through the passive interface
- Battery voltage and approximate state-of-charge reporting
- Maintenance Wi-Fi, browser dashboard, live console, and AT terminal
- Persistent event logging and cellular-network clock synchronization
- North American reorder and receiver-off-hook warning tones

The prototype has operated successfully in normal use and during public demonstration.

## Known prototype failure

After approximately one hour of operation during an August 25 field test, the modem remained registered but stopped originating and receiving calls. Incoming calls went to voicemail, outgoing calls returned `+CME ERROR: no network service`, and service code `9999` did not recover the modem. Cycling the LilyGO board's physical power switch restored incoming calls.

This is the reason the A4 audio board includes a hardware power-cycle circuit. Firmware v0.10.4 does **not** yet drive that new reset input.

## Ordered hardware awaiting validation

| Assembly | Ordered package | Status |
| --- | --- | --- |
| Passive audio and reset PCB | `hardware/audio-reset-a4/` | Ordered August 27; unassembled and untested |
| AG1171 through-hole carrier | `hardware/ag1171-carrier-through-hole/` | Ordered August 28; unassembled and untested |

The Audio and Reset A4 board is approximately 35.00 x 25.15 mm. The notched AG1171 carrier has an overall Gerber envelope of approximately 54.91 x 42.67 mm.

## Next validation milestone

1. Inspect received boards against the Gerber render and connector orientation.
2. Check power and ground continuity before installing modules.
3. Assemble and test the audio board without enabling hardware reset.
4. Verify transmit/receive audio and trimmer ranges.
5. Test the reset one-shot with the LilyGO disconnected from the telephone.
6. Add firmware control of the reset trigger and verify recovery behavior.
7. Install both boards in the telephone and repeat incoming, outgoing, ringing, charging, and endurance tests.

Until these steps pass, the ordered PCBs should be considered **candidate production hardware**.
