# Known Issues — v0.9

- Maintenance Wi-Fi is intentionally temporary and shuts down 10 minutes after it is enabled, even if a browser remains connected.
- The full serial history is RAM-backed and therefore does not survive a reset or complete power loss. Important events are stored separately in LittleFS.
- Real timestamps depend on the cellular network supplying a valid `+CCLK` value. Until then, logs are timestamped by uptime.
- The web console uses HTTP polling rather than WebSockets. This avoids an external Arduino dependency but can introduce several seconds of display latency for output generated between polls.
- The default development Wi-Fi password is not intended as strong long-term security. Change it in `Config.h` when appropriate.
- Battery percentage remains an estimate derived from cell voltage and can move under load.
