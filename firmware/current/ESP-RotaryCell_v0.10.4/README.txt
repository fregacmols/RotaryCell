ESP-RotaryCell v0.10.4
===================

Baseline
--------
v0.9.1 is based directly on the known-working v0.8 telephone firmware. The
existing AG1171 hook/pulse input, physical ringing, incoming/outgoing cellular
calls, dial tone, six-second dial completion timeout, safety blocks, battery
monitoring, and USB serial console are retained.

New in v0.9.1
-----------
1. Dial 0000 while off hook to toggle maintenance Wi-Fi.
2. Maintenance Wi-Fi starts a WPA2 access point named ESP-RotaryCell.
3. Default password: rotarycell
4. The access point automatically shuts off 10 minutes after activation.
5. Browse to http://192.168.4.1 while connected to the access point.
6. The browser dashboard shows battery, cellular registration/operator/signal,
   handset state, phone state, modem state, clock status, and Wi-Fi time left.
7. The browser console shows the pre-existing RAM serial history and then live
   console output. Commands entered there use the same Console object as USB.
8. AT terminal mode is available from the browser exactly as from USB: enter A,
   send AT commands, then enter EXIT.
9. A 32 KiB RAM ring buffer retains recent console output generated before the
   browser connected. It is intentionally non-persistent and clears on reset.
10. A bounded LittleFS event log retains important events across resets.
11. +CCLK network time from the A7670 is parsed and used for real timestamps.
    Before network time is available, log lines use uptime timestamps.
12. The cellular clock is retried every minute until synchronized and refreshed
    every six hours while the modem is idle.
13. Low-battery threshold transitions are written to the persistent event log.

Maintenance Wi-Fi
----------------
Lift the handset and rotary-dial 0000. The code is intercepted locally as soon
as the fourth zero is completed and is never sent to the cellular modem. Wi-Fi
remains available after the handset is replaced. Dial 0000 again, use the web
"Turn Wi-Fi off" button, or wait 10 minutes to shut it down.

The default AP password is deliberately easy to type during development. Change
WIFI_AP_PASSWORD in Config.h before using the phone where untrusted people could
join the temporary access point.

Browser console implementation
------------------------------
The web terminal uses the ESP32 Arduino core's built-in WebServer and short HTTP
polling, not an external WebSocket library. This keeps v0.9.1 self-contained: no
additional Arduino library should be required beyond the ESP32 core.

Persistent logging
------------------
The serial-history ring buffer is 32 KiB and receives the same output that goes
to USB Serial. Once it fills, the oldest bytes are overwritten.

Important lifecycle events are separately appended to /events.log in LittleFS.
The event log rotates when it reaches approximately 64 KiB. The web interface
can display and clear it.

Clock behavior
--------------
The A7670 is queried with AT+CCLK?. Valid network time is converted to UTC while
preserving the modem-provided local timezone offset for display. Obviously stale
default dates before 2024 are rejected. Until synchronization succeeds, log
prefixes use uptime, for example [+0:02:31].

Required board support
----------------------
ESP32 Arduino core with WiFi, WebServer, and LittleFS support (all standard for
ESP32-S3 builds).

v0.10.1 additions
---------------
- Dial 9999 while idle to capture a persistent pre-restart A7670 diagnostic snapshot,
  restart/recover the modem, then capture a matching post-restart snapshot.
- Diagnostic snapshots include raw command/response logging for CPIN, CSQ, COPS,
  CREG/CGREG/CEREG, CLCC, CGATT, CPSI, CNMP, CCLK and firmware/RI state.
- Maintenance web page now accepts an Arduino-exported application .bin for OTA.
  OTA requires an OTA-capable ESP32 partition scheme (two app slots).
- Dial 0000 remains the maintenance Wi-Fi toggle.


v0.10.1 diagnostic logging changes
----------------------------------
- Service code 9999 now explicitly saves ESP-side state (phone state, handset,
  uptime, and battery) before restarting the modem.
- The complete raw pre- and post-restart AT snapshots are written to the
  persistent LittleFS event log, including each command and its response.
- Removed the A7670G diagnostic query that returned ERROR in the v0.10 test.
- The maintenance page now refreshes the persistent event log automatically
  every 3 seconds; the Refresh events button remains available.

Dashboard readability update
----------------------------
- Cellular status now prefers +CEREG (LTE/EPS registration) so A7670G +CREG status 7 does not overwrite a valid LTE roaming registration.
- Registered states display as REGISTERED (HOME) or REGISTERED (ROAMING).
- Dashboard label changed from Operator to Network.
- Common U.S. PLMN codes are translated to T-Mobile, AT&T, Verizon, US Cellular, or AT&T / FirstNet; unknown codes remain numeric.

v0.10.2 rotary-dial timing fix
------------------------------
- Removed the v0.10.1 automatic 3-second persistent-event-log refresh.
- The ESP32 WebServer is synchronous and /api/events reads LittleFS. Repeated browser
  event-log reads could briefly delay the polling-based SHK rotary decoder enough to
  miss a dial pulse (for example, a dialed 9 being counted as 8).
- Persistent events are still saved exactly as in v0.10.1. Use the Refresh events
  button when you want to display them; live console/status polling remains unchanged.

v0.10.3 dialing responsiveness
------------------------------
- Reduced the completed-number interdigit timeout from 6 seconds to 3 seconds.
- Seven-digit numbers remain unchanged and are passed directly to the cellular modem;
  the firmware does not prepend or infer an area code.

v0.10.4 call-progress and permanent-signal tones
------------------------------------------------
- Added North-American reorder tone: 480 + 620 Hz, 250 ms on / 250 ms off.
- Added receiver-off-hook warning (howler): 1400 + 2060 + 2450 + 2600 Hz,
  100 ms on / 100 ms off.
- An untouched off-hook line now gives dial tone for 15 seconds, reorder for
  10 seconds, then the howler until the handset is replaced.
- Incomplete/blocked numbers and ended or failed outgoing calls enter reorder
  treatment instead of returning immediately to fresh dial tone.
- Dial pulses are ignored during reorder and howler treatment; hanging up
  clears the treatment and restores the normal idle state.
