# Changelog

## v0.8 — Baseline integrated release

- Added explicit phone states: idle, incoming, dial tone, collecting digits, outgoing, and connected.
- Retained the tested six-second number-completion timeout.
- Added local dialing safeguards before `ATD`:
  - blocks exact `911`;
  - blocks `900...` and `1900...` premium-rate prefixes;
  - rejects numbers shorter than seven digits.
- Added handling for `BUSY`, `NO ANSWER`, `NO CARRIER`, and `NO DIALTONE`.
- Added outgoing-call cancellation when the handset is replaced.
- Stops physical ringing if an incoming caller disconnects before answer.
- Preserved incoming ringing, automatic answer/hangup, rotary pulse decoding, battery reporting, AT terminal, and manual diagnostics.
- Integrated the working transformer-based hardware as Hardware Rev A.

## v0.7.1

- Increased number-completion timeout from three seconds to six seconds.
- Printed the exact accumulated number and `ATD` command before dialing.

## v0.7

- Added outgoing rotary dialing and number accumulation.
- Added GPIO36 tone-generator support in firmware.

## v0.6

- Enabled the LilyGO external audio amplifier with modem GPIO3 commands.
- Reasserted audio routing and gain when answering calls.
- Established working two-way audio.

## v0.5.1

- Changed handset hangup handling to `AT+CHUP`.

## v0.5

- Added automatic incoming-call answer and handset hangup behavior.

## v0.4

- Added incoming-call detection and physical ring-cadence testing.
- Preserved separate experimental branches for automatic-polling behavior.

## v0.3

- Added battery-voltage monitoring and status reporting.

## v0.2

- Added modem and telephone diagnostics.

## v0.1

- Initial modular firmware skeleton.

## v0.9 — Maintenance Wi-Fi and diagnostics
- Added local rotary service code `0000` to toggle maintenance Wi-Fi.
- Added 10-minute WPA2 maintenance AP and browser status dashboard.
- Added browser-accessible interactive console sharing the USB command/AT parser.
- Added 32 KiB RAM serial-history ring buffer.
- Added bounded LittleFS persistent event log.
- Added A7670 `AT+CCLK?` cellular-network timestamp synchronization and periodic refresh.
- Added low-battery transition events to the persistent log.
- Preserved v0.8 telephone behavior for ordinary calls and dialing.
