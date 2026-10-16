# v0.12.0 bench validation

Record phone hardware, modem firmware, carrier, and application binary hash for
each run. Keep the supported v0.11.1-dev application available for rollback.

1. **Regression:** outgoing and incoming calls, rotary decoding, handset audio,
   answer/hang-up, ringing, battery status, maintenance 0000, USB WIFI, persistent
   diagnostics, SD downloads, OTA reopening, and battery-only 9999 reset.
2. **Filters:** independently exercise each direction/mode with matching and
   nonmatching numbers; empty whitelist; default and selectable restrictions;
   formatted/+1 equivalents; seven-digit versus full numbers; service codes.
   Use controlled test numbers: verify 911 blocking without placing a real call.
3. **Identity:** capture complete raw CLIP responses for ordinary, withheld, and
   unavailable caller IDs. Test late CLIP, CLIP preceding RING, no CLIP, and
   quoted alpha names. Identified blocked callers must never ring or auto-answer,
   including when the handset was already off hook. With unknown calls allowed,
   late blocked identity must stop ringing and prevent pickup from answering.
   Allowed calls must remain answerable.
4. **History:** verify confirmed connection, missed/unconfirmed, busy, failed,
   blocked, cancelled, remote/local hang-up, and DND flags. Verify latest-100
   rotation, restart persistence, and list additions. Clear History must leave
   settings intact. Interrupt a save on a test phone and confirm older records
   remain readable. Check history after prolonged modem/AT diagnostics.
5. **DND:** test manual 0002 and web settings; overnight and daytime schedules;
   start inclusive/end exclusive; return to active/inactive schedule; missing
   clock; reboot; spring/fall DST; schedule transition during an incoming call.
   No bell while active; pickup still answers; outgoing calling remains normal.
6. **Credentials:** save without disconnecting, apply and reconnect, restart
   persistence, invalid SSID/password, empty password preservation, 0001 double
   entry, timeout/hang-up cancellation, incoming-call cancellation, and tones.
   Confirm filters/DND/history survive Wi-Fi credential recovery.
7. **Power:** test each interval and Always On; pickup just before/after a poll;
   sustained off-hook/dialing; incoming-call immediate wake; no missed pulse or
   false hang-up; persisted preferences; verify measured current reduction.
8. **UI:** exercise all six sections on desktop and phone browsers, list editing,
   unsaved edits during status refresh, save failures, and reconnect behavior.

Any failed connection-state, identity, or filesystem test blocks promotion.
