# Maintenance Wi-Fi Guide — v0.9

## Enable

1. Lift the handset.
2. Dial `0000` on the rotary dial.
3. The firmware intercepts the code locally after the fourth digit and starts the maintenance access point.
4. Replace the handset if desired; Wi-Fi remains on until its timer expires.

## Connect

- SSID: `ESP-RotaryCell`
- Default password: `rotarycell`
- Address: `http://192.168.4.1`

The access point is not an Internet connection. A phone may warn that the Wi-Fi network has no Internet; remain connected anyway.

## Dashboard

The page reports battery voltage/estimated percentage, modem status, cellular registration, operator, RSSI, handset state, phone state, network-clock status, and the remaining maintenance-Wi-Fi time.

## Console

The console initially loads recent serial output retained in the 32 KiB RAM ring buffer, including messages generated before Wi-Fi was enabled. It then polls for new output.

Commands are the same as the USB serial console. Enter `A` to enter AT terminal mode and `EXIT` to leave it.

## Persistent events

The event panel reads the LittleFS event log. Important events survive a reset or power cycle, unlike the full serial-history buffer. The log rotates automatically at approximately 64 KiB.

## Disable

Any of these will shut maintenance Wi-Fi down:

- dial `0000` again;
- press **Turn Wi-Fi off** on the web page;
- wait 10 minutes from activation.

## Security

The default development password is stored in `Config.h`. Change `WIFI_AP_PASSWORD` if the phone will be used around people who should not have access to the modem AT console.
