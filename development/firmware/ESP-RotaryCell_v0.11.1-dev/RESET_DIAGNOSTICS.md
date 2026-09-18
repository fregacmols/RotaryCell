# v0.11.1-dev reset diagnostics

Derived from v0.11.0-dev; original retained. Both snapshots add raw AT+CGMR and AT+CIREG? replies, including ERROR/timeouts, to the existing diagnostic queries.

9999 logs an ID and stores a pending marker in NVS before the power-cut trigger. On the next boot, a matching post-reset snapshot runs at the first idle/on-hook opportunity at least 30 seconds after setup completes. It does not wait for registration to succeed, so failures remain visible. AT terminal mode defers the snapshot: enter EXIT and hang up to allow it. A call interrupts the snapshot between queries and defers a retry. Each existing query has a 1.5-second timeout, so this is not fully asynchronous and brief response delays remain possible.

If the trigger returns without a power cut, the pending marker is cleared and the existing continued-power message is logged. NVS failure is logged; manual reset still proceeds. A marker associates the next boot with a request, but does not independently prove the power waveform or that voice calling works. Inspect the logged reset reason and test a call.

No automatic fault detection/reset has been enabled. Development power logging retains the previous build setting. Hardware validation remains required.
