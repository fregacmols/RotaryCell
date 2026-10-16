# RotaryCell v0.12.0 — first usability build

Usability release based on v0.11.1-dev. As of October 16, 2026, the owner reports
several days of use on two phones with no notable problems. This is the working
0.12 baseline; bug fixes and minor feature additions or changes will use 0.12.x
revisions. The release removes the former `-dev` suffix from the source,
displayed version, and OTA filename. The public OTA image also disables optional
INA226/microSD diagnostic power logging. Field use reported above
was on the earlier development-labelled image, not this rebuilt image.

The guide firmware and downloads remain v0.11.1-dev. The bench checklist below
remains available for targeted regression checks; its edge cases have not all
been individually verified.

## Changes

- Maintenance navigation: Overview, Calls, Phone Settings, Power Saving,
  Diagnostics, System. Existing console, event/SD downloads, OTA, and Wi-Fi
  countdown controls are retained.
- Incoming/outgoing Off, Whitelist, and Blacklist modes, independent lists of
  up to 50 numbers, and selectable private/unavailable caller-ID policies.
- Selectable outgoing 911, North American 900/1-900, and minimum-length
  restrictions. All applicable restrictions must pass; disabling the 911 rule
  alone does not exempt that number from the minimum-length rule or whitelist.
- Latest 100 persistent call records, including blocked attempts. Select recent
  numbers into the corresponding list, then save. Clear History leaves lists
  and other settings intact. Service codes do not create call records.
- Manual and overnight daily DND silences the bell without rejecting allowed
  calls. Manual mode overrides the schedule until disabled. Schedule evaluation
  pauses without a synchronized clock. Default schedule is disabled; the form
  initially offers 22:00–06:00.
- Saved phone time zone controls schedules, displayed time, and history. Initial
  default is US Eastern. Initial choices cover UTC and US time zones, including
  Arizona and Hawaii; applicable US zones include daylight-saving rules.
- Saved maintenance SSID/password. Blank password keeps the current password.
  Save does not disconnect the browser; Apply restarts the AP after replying.
- AG1171 idle saving defaults on, with 500 ms, 1 s, and 2 s sleep intervals or
  Always On. Wake settling remains 60 ms and hook sampling remains 20 ms, so the
  complete idle polling cycle is longer than the selected sleep interval.

## Local service codes and tones

| Code | Action / feedback |
| --- | --- |
| 0000 | Existing maintenance Wi-Fi toggle |
| 0001, first entry | Three short handset tones; arm Wi-Fi credential reset |
| 0001, second within 30 seconds | Reset only SSID/password, start AP; four tones |
| 0002 | Toggle manual DND: one tone enabled, two tones disabled |
| 0002, manual disabled while schedule active | Three tones: scheduled DND still active |
| 9999 | Existing paired-diagnostic hardware power cycle |

Hanging up, an incoming call, or expiry cancels an armed Wi-Fi reset. A failed
settings save produces three lower/reorder-frequency tones. Feedback is driven
by the existing GPIO36 timer without blocking handset/modem polling. Defaults
after credential reset are `ESP-RotaryCell` / `rotarycell`.

## Call identity and outcomes

Filtering waits up to two seconds for caller ID before ringing or answering.
Private/unavailable distinctions use the optional sixth `+CLIP` CLI-validity
field. If it is absent and no usable number is supplied, classify the call as
unknown; do not guess that it was deliberately withheld. Hardware/carrier
verification of these fields is outstanding. Quoted commas in alpha text are
handled by the parser.

If a number arrives after the two-second decision timeout, filtering is checked
again while the call remains unanswered. An allowed unknown call may already
have rung; a newly blocked identity stops the bell and rejects the call before
pickup processing. Set unknown callers to Block if this possibility is unwanted.

Number matching strips common formatting and treats an eleven-digit leading-1
number as equivalent to its North American ten-digit form. Other representations
must match exactly; seven-digit local numbers are not equated with arbitrary
area codes. This initial normalization is intended for North American use.

`AT+CLCC` polls during calls confirm connection and detect an empty call list.
Sending ATD/ATA alone does not count as a connected call. Missed versus unconfirmed
is explicitly identified when connection evidence is unavailable. The raw AT
terminal is an expert diagnostic interface and can bypass normal phone controls;
avoid issuing other commands during call-state validation.

## Persistence

Versioned settings use ESP32 Preferences/NVS. Call history uses separate rotating
LittleFS files, checksums, and a temporary-file rename; completed pending writes
are recovered on boot. Previous records remain when a pending write is incomplete.
Unfinished calls can be lost if power is removed before the final record is saved.
Read operations do not save history. Event logs are separate from call history.

This candidate does not automatically format LittleFS after a mount failure,
to avoid erasing existing records. It expects the initialized filesystem used by
v0.11.1-dev. If unavailable, the overview reports history storage unavailable;
fresh-device filesystem initialization needs a separate deliberate procedure.

## Build and validation

Use the v0.11.1 build environment in the root `FIRMWARE_BUILDING.md`: ESP32 core
3.3.11, ESP32S3 Dev Module, 16 MB QIO, `min_spiffs` with OTA, hardware CDC, no PSRAM.
The public build uses `ENABLE_DEVELOPMENT_POWER_LOGGING = false`: the INA226 and
microSD power logger are not initialized, and their diagnostic status panel is
hidden. Instrumented test phones can opt in by setting it true and rebuilding.
The logger's pre-existing zero voltage/power-field issue remains unresolved.

Automated checks:

- `node tests/maintenance-ui.test.cjs`: executes actual embedded JavaScript with
  DOM/network doubles, covering navigation, setting population, unchecked
  restrictions, safe history rendering, list additions, missing clock status,
  password clearing, explicit Wi-Fi apply, and Clear History.
- `PolicyChecks.cpp`: compiled assertions cover real policy functions, DND
  start/end boundaries, overnight ranges, and CLI-validity parsing.

See `BENCH_TESTS.md` for required hardware checks. Successful compilation and
these tests do not establish modem or assembled-phone validation.
