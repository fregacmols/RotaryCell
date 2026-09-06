# Project status

**Baseline date:** September 6, 2026

**Firmware:** v0.10.4
**Hardware phase:** First integrated PCB assembly operational; extended features awaiting firmware validation

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

## First integrated PCB assembly

| Assembly | Ordered package | Current status |
| --- | --- | --- |
| Passive audio and reset PCB | `hardware/audio-reset-a4/` | Received and assembled; existing v0.10.4 audio functions passed initial testing; automatic-reset function not yet controlled or validated |
| AG1171 through-hole carrier | `hardware/ag1171-carrier-through-hole/` | Received and assembled; existing v0.10.4 telephone functions passed initial testing; AG1171 power-down control not yet implemented or validated |

![First assembled RotaryCell PCBs connected to the LilyGO for bench testing](docs/images/first-jlc-build/assembled-pcbs-bench-test.jpg)

*First assembled Audio/Reset A4 and AG1171 carrier boards connected to the LilyGO during initial functional bench testing. This shows the development setup and loose test wiring, not the final installation or an authoritative wiring reference.*

The Audio and Reset A4 board is approximately 35.00 x 25.15 mm. The notched AG1171 carrier has an overall Gerber envelope of approximately 54.91 x 42.67 mm.

## Next validation milestone

1. Record clear photographs, fitted component values, polarities, connector orientations, and any assembly rework from the first build.
2. Record repeatable measurements and individual pass/fail results for audio, dialing, hook detection, calling, ringing, and charging.
3. Test the reset one-shot independently before enabling it from the LilyGO.
4. Add firmware control of the reset trigger and verify full power-cycle recovery.
5. Add firmware control of the AG1171 power-down input and verify startup, shutdown, and idle behavior.
6. Repeat incoming, outgoing, ringing, charging, battery, and endurance tests on the complete assembly.

Until these steps pass, the ordered PCBs should be considered **working development hardware**, not a production-qualified design.
