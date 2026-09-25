# Maintenance access and downloads

In the ESP USB console, send `WIFI ON` (or `WIFI`) followed by Enter.
If in AT terminal mode, send `EXIT` first. Existing `W` still pulses modem PWRKEY.
Connect to ESP-RotaryCell (default password `rotarycell`) and open http://192.168.4.1.
Wi-Fi expires after 10 minutes; repeating the command renews that timer.

The maintenance page offers Download event log for the persistent ESP32 event
history, and an SD power logs section listing POWER001.CSV through POWER999.CSV.
Refresh the list, then click a file to download it. SD storage must be initialized
by the optional development power logger; absent/disabled storage displays an error.

CSV downloads flush pending samples, close the active file, and directly stream
a fixed-length snapshot to the browser. Power sampling pauses during the
synchronous transfer and resumes in the same file afterward. A measured download
of an approximately 19 MB log caused a 44.9-second gap in that log. This is
acceptable for maintenance use because the phone is not being actively evaluated
during a download, but it is a known limitation for future asynchronous work.
An interrupted download can be retried; no files are deleted.

Validation: firmware compilation and physical Wi-Fi/SD downloads have passed.

Use Keep Wi-Fi on to disable the countdown for this maintenance session.
Start 10-minute countdown restores it with a fresh timer. Turning Wi-Fi off
and back on, or rebooting, restores timed operation.

After a successful web firmware update, a one-shot saved flag requests maintenance
Wi-Fi on the next boot so the browser can reconnect without dialing `0000`. Normal
boots retain the usual timed/off behavior.
