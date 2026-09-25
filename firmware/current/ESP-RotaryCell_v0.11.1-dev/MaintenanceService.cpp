#include "MaintenanceService.h"
#include "Config.h"
#include "PhoneLine.h"
#include "BatteryMonitor.h"
#include "Modem.h"
#include "Console.h"
#include "EventLog.h"
#include "ClockManager.h"
#include "HardwareControl.h"
#include "PowerLogger.h"
#include <WiFi.h>
#include <SD.h>
#include <Update.h>
#include <Preferences.h>
#include <esp_system.h>
#include "LogSerial.h"

namespace {
constexpr char MAINTENANCE_PREFS_NAMESPACE[] = "maintenance";
constexpr char WIFI_AFTER_OTA_KEY[] = "wifiAfterOta";

bool requestWifiAfterOta() {
  Preferences preferences;
  if (!preferences.begin(MAINTENANCE_PREFS_NAMESPACE, false)) return false;
  const bool saved = preferences.putBool(WIFI_AFTER_OTA_KEY, true) == sizeof(uint8_t);
  preferences.end();
  return saved;
}

bool takeWifiAfterOtaRequest() {
  Preferences preferences;
  if (!preferences.begin(MAINTENANCE_PREFS_NAMESPACE, false)) return false;
  const bool requested = preferences.getBool(WIFI_AFTER_OTA_KEY, false);
  if (requested) preferences.remove(WIFI_AFTER_OTA_KEY);
  preferences.end();
  return requested;
}
}

static const char PAGE_HTML[] PROGMEM = R"HTML(
<!doctype html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP-RotaryCell</title>
<style>
body{font-family:system-ui,sans-serif;margin:0;background:#111;color:#eee}main{max-width:1000px;margin:auto;padding:18px}.card{background:#1d1d1d;border:1px solid #333;border-radius:10px;padding:14px;margin-bottom:14px}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:8px}.metric{background:#282828;border-radius:7px;padding:10px}.k{color:#aaa;font-size:.8rem}.v{font-size:1.08rem;margin-top:3px}pre{background:#050505;color:#e7e7e7;padding:12px;border-radius:7px;height:42vh;overflow:auto;white-space:pre-wrap;word-break:break-word}input{width:calc(100% - 110px);padding:9px;background:#090909;color:#fff;border:1px solid #555;border-radius:5px}button{padding:9px 12px;margin:3px;border:0;border-radius:5px;cursor:pointer}.danger{background:#8b2525;color:#fff}.small{color:#aaa;font-size:.85rem}h1{margin-top:0}</style></head>
<body><main><h1 id="heading">ESP-RotaryCell</h1>
<div class="card"><div class="grid" id="status"></div><div class="small" id="countdown"></div><button id="wifiTimer">Keep Wi-Fi on</button></div>
<div class="card"><h2>Experimental AG1171 power saving</h2><p class="small">When enabled, the AG1171 sleeps while the telephone is idle and wakes periodically to check SHK. This setting returns to OFF after every reboot.</p><button id="powerSaving">Enable power saving</button><p class="small">Dial 9999 to capture diagnostics and trigger the Rev A4 hardware power cycle. Disconnect USB first so battery removal can reset the complete LilyGO board.</p></div>
<div class="card" id="devPower" style="display:none"><h2>Development power logger</h2><p id="devPowerStatus"></p><p class="small">INA226 measurements are recorded at 10 Hz to the LilyGO microSD card. This panel and the instrumentation are controlled by ENABLE_DEVELOPMENT_POWER_LOGGING in Config.h.</p></div>
<div class="card"><h2>Live console</h2><pre id="console"></pre><form id="cmd"><input id="input" autocomplete="off" placeholder="S, M, A, AT command, EXIT..."><button>Send</button></form><button id="clearRam">Clear RAM history</button></div>
<div class="card"><h2>Persistent event log</h2><pre id="events" style="height:24vh"></pre><a href="/download/events"><button>Download event log</button></a><button id="refreshEvents">Refresh events</button><button id="clearEvents">Clear events</button><button class="danger" id="wifiOff">Turn Wi-Fi off</button></div>
<div class="card"><h2>SD power logs</h2><button id="refreshFiles">Refresh file list</button><div id="powerFiles"></div><p id="downloadProgress" class="small"></p></div>
<div class="card"><h2>Firmware update</h2><p class="small">Upload the application .bin exported by Arduino IDE. The phone will reboot automatically after a successful update.</p><form method="POST" action="/update" enctype="multipart/form-data"><input type="file" name="firmware" accept=".bin,application/octet-stream" required><button type="submit">Install firmware</button></form></div>
<script>
let cursor=0;const con=document.getElementById('console');
async function syncBrowserTime(){try{const d=new Date();await fetch('/api/time',{method:'POST',headers:{'Content-Type':'text/plain'},body:String(Date.now())+','+String(d.getTimezoneOffset())});}catch(e){}}
function esc(s){return String(s??'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));}
async function status(){try{let r=await fetch('/api/status');let s=await r.json();document.getElementById('heading').textContent='ESP-RotaryCell v'+s.version;let rows={Battery:s.battery,Cellular:s.registration,Network:s.operator,Signal:s.signal,Handset:s.handset,State:s.phoneState,Modem:s.modem,AG1171:s.ag1171PowerState,PowerSaving:s.powerSaving?'ENABLED':'OFF',Clock:s.clock,ClockSource:s.clockSource};document.getElementById('status').innerHTML=Object.entries(rows).map(([k,v])=>`<div class="metric"><div class="k">${esc(k)}</div><div class="v">${esc(v)}</div></div>`).join('');document.getElementById('powerSaving').textContent=s.powerSaving?'Disable power saving':'Enable power saving';let d=document.getElementById('devPower');d.style.display=s.developmentPowerLoggingEnabled?'block':'none';if(s.developmentPowerLoggingEnabled)document.getElementById('devPowerStatus').textContent=s.developmentPowerStatus;document.getElementById('countdown').textContent=s.wifiTimeoutEnabled?`Maintenance Wi-Fi shuts off in ${s.wifiSecondsRemaining}s`:'Maintenance Wi-Fi will stay on until turned off';document.getElementById('wifiTimer').textContent=s.wifiTimeoutEnabled?'Keep Wi-Fi on':'Start 10-minute countdown'; }catch(e){}}
async function logs(){try{let r=await fetch('/api/log?since='+cursor);let start=Number(r.headers.get('X-Log-Start')||0),end=Number(r.headers.get('X-Log-End')||start),t=await r.text();if(start>cursor){con.textContent='[Earlier RAM log data was overwritten]\n'+t;}else con.textContent+=t;cursor=end;con.scrollTop=con.scrollHeight;}catch(e){}}
async function events(){try{document.getElementById('events').textContent=await (await fetch('/api/events')).text();}catch(e){}}
document.getElementById('cmd').onsubmit=async e=>{e.preventDefault();let i=document.getElementById('input');let v=i.value;i.value='';await fetch('/api/command',{method:'POST',headers:{'Content-Type':'text/plain'},body:v});};
document.getElementById('clearRam').onclick=async()=>{await fetch('/api/log/clear',{method:'POST'});cursor=0;con.textContent='';};
document.getElementById('powerSaving').onclick=async()=>{await fetch('/api/power-saving/toggle',{method:'POST'});status();};
document.getElementById('wifiTimer').onclick=async()=>{try{let r=await fetch('/api/wifi/timer',{method:'POST'});if(!r.ok)throw Error('Unable to change timer');await status();}catch(e){alert(e.message);}};
document.getElementById('refreshEvents').onclick=events;
document.getElementById('clearEvents').onclick=async()=>{if(confirm('Clear persistent event log?')){await fetch('/api/events/clear',{method:'POST'});events();}};
document.getElementById('wifiOff').onclick=async()=>{if(confirm('Turn maintenance Wi-Fi off now?'))await fetch('/api/wifi/off',{method:'POST'});};
async function powerFiles(){const panel=document.getElementById('powerFiles');try{let r=await fetch('/api/power-files');if(!r.ok)throw Error(await r.text());let files=await r.json();panel.replaceChildren();if(!files.length)panel.textContent='No power CSV files found.';for(const f of files){let a=document.createElement('a');a.href='/download/power?name='+encodeURIComponent(f.name);a.download=f.name;let b=document.createElement('button');b.textContent=`Download ${f.name} (${f.size} bytes)`;a.appendChild(b);panel.appendChild(a);}}catch(e){panel.textContent=e.message;}}
document.getElementById('refreshFiles').onclick=powerFiles;powerFiles();
syncBrowserTime().then(status);logs();events();setInterval(status,2000);setInterval(logs,7000);
</script></main></body></html>)HTML";

MaintenanceService::MaintenanceService(PhoneLine& phoneLine,
                                       BatteryMonitor& batteryMonitor,
                                       Modem& modem,
                                       Console& console,
                                       EventLog& eventLog,
                                       ClockManager& clock,
                                       HardwareControl& hardwareControl,
                                       PowerLogger& powerLogger)
  : _phoneLine(phoneLine), _batteryMonitor(batteryMonitor), _modem(modem),
    _console(console), _eventLog(eventLog), _clock(clock),
    _hardwareControl(hardwareControl), _powerLogger(powerLogger),
    _server(WEB_SERVER_PORT) {}

void MaintenanceService::begin() {
  WiFi.mode(WIFI_OFF);
  configureRoutes();
  const bool requestedAfterOta = takeWifiAfterOtaRequest();
  const bool upgradingFromEarlierFirmware = esp_reset_reason() == ESP_RST_SW;
  if (requestedAfterOta || upgradingFromEarlierFirmware) {
    LogSerial.println("WIFI: reopening maintenance access after OTA update");
    _eventLog.add("Maintenance Wi-Fi reopened automatically after OTA update");
    enable();
  }
}

void MaintenanceService::configureRoutes() {
  _server.on("/", HTTP_GET, [this]() { handleRoot(); });
  _server.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
  _server.on("/api/log", HTTP_GET, [this]() { handleLog(); });
  _server.on("/api/power-files", HTTP_GET, [this]() { handlePowerFiles(); });
  _server.on("/download/power", HTTP_GET, [this]() { handlePowerDownload(); });
  _server.on("/download/events", HTTP_GET, [this]() {
    _server.sendHeader("Content-Disposition", "attachment; filename=RotaryCell-events.txt");
    handleEvents();
  });
  _server.on("/api/events", HTTP_GET, [this]() { handleEvents(); });
  _server.on("/api/command", HTTP_POST, [this]() { handleCommand(); });
  _server.on("/api/time", HTTP_POST, [this]() { handleTimeSync(); });
  _server.on("/api/log/clear", HTTP_POST, [this]() { handleClearRamLog(); });
  _server.on("/api/events/clear", HTTP_POST, [this]() { handleClearEvents(); });
  _server.on("/api/wifi/timer", HTTP_POST, [this]() {
    _timeoutEnabled = !_timeoutEnabled;
    _enabledAt = millis();
    _eventLog.add(_timeoutEnabled ? "Wi-Fi countdown enabled (10 minutes)" : "Wi-Fi countdown disabled");
    _server.send(200, "text/plain", _timeoutEnabled ? "Countdown enabled" : "Countdown disabled");
  });
  _server.on("/api/wifi/off", HTTP_POST, [this]() { handleWifiOff(); });
  _server.on("/api/power-saving/toggle", HTTP_POST, [this]() { handlePowerSavingToggle(); });
  _server.on("/update", HTTP_POST,
    [this]() { handleOtaComplete(); },
    [this]() { handleOtaUpload(); });
  _server.onNotFound([this]() { _server.sendHeader("Location", "/", true); _server.send(302, "text/plain", ""); });
}

void MaintenanceService::enable() {
  if (_enabled) {
    _timeoutEnabled = true;
    _enabledAt = millis();
    return;
  }

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD)) {
    LogSerial.println("WIFI: failed to start maintenance access point");
    _eventLog.add("WIFI failed to start");
    WiFi.mode(WIFI_OFF);
    return;
  }
  _console.resetWebSession();
  _server.begin();
  _modem.requestStatus();
  _enabled = true;
  _timeoutEnabled = true;
  _enabledAt = millis();

  LogSerial.println("WIFI: maintenance access point ON for 10 minutes");
  LogSerial.print("WIFI: SSID = "); LogSerial.println(WIFI_AP_SSID);
  LogSerial.print("WIFI: address = http://"); LogSerial.println(WiFi.softAPIP());
  _eventLog.add("Maintenance Wi-Fi enabled");
}

void MaintenanceService::disable() {
  if (!_enabled) return;
  LogSerial.println("WIFI: maintenance access point OFF");
  _eventLog.add("Maintenance Wi-Fi disabled");
  _console.resetWebSession();
  _server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  _enabled = false;
  _enabledAt = 0;
}

void MaintenanceService::toggle() {
  if (_enabled) disable(); else enable();
}

void MaintenanceService::update() {
  if (_console.takeWifiRequest()) enable();
  if (!_enabled) return;
  _server.handleClient();
  if (_timeoutEnabled && millis() - _enabledAt >= WIFI_MAINTENANCE_TIMEOUT_MS) {
    LogSerial.println("WIFI: 10-minute maintenance timeout reached");
    disable();
  }
}

bool MaintenanceService::isEnabled() const { return _enabled; }

uint32_t MaintenanceService::secondsRemaining() const {
  if (!_enabled || !_timeoutEnabled) return 0;
  const uint32_t elapsed = millis() - _enabledAt;
  if (elapsed >= WIFI_MAINTENANCE_TIMEOUT_MS) return 0;
  return (WIFI_MAINTENANCE_TIMEOUT_MS - elapsed + 999) / 1000;
}

void MaintenanceService::setPhoneState(const char* stateName) {
  _phoneState = stateName ? stateName : "UNKNOWN";
}

String MaintenanceService::jsonEscape(const String& value) {
  String out;
  out.reserve(value.length() + 8);
  for (size_t i=0;i<value.length();++i) {
    const char c=value[i];
    if (c=='"' || c=='\\') { out+='\\'; out+=c; }
    else if (c=='\n') out+="\\n";
    else if (c=='\r') out+="\\r";
    else out+=c;
  }
  return out;
}

String MaintenanceService::statusJson() const {
  String battery = "Unavailable";
  if (_batteryMonitor.readingValid()) {
    battery = String(_batteryMonitor.volts(), 3) + " V / " + String(_batteryMonitor.estimatedPercent()) + "%";
  }
  String signal = (_modem.rssi() < 0 || _modem.rssi() == 99)
                    ? String("Unknown")
                    : String(_modem.rssi()) + " / 31";
  String operatorName = _modem.operatorName().length() ? _modem.operatorName() : String("Unknown");

  String json = "{";
  json += "\"wifiTimeoutEnabled\":" + String(_timeoutEnabled ? "true" : "false") + ",";
  json += "\"version\":\"" + String(FIRMWARE_VERSION) + "\",";
  json += "\"battery\":\"" + jsonEscape(battery) + "\",";
  json += "\"batteryMv\":" + String(_batteryMonitor.millivolts()) + ",";
  json += "\"batteryPercent\":" + String(_batteryMonitor.estimatedPercent()) + ",";
  json += "\"registration\":\"" + jsonEscape(_modem.registrationText()) + "\",";
  json += "\"operator\":\"" + jsonEscape(operatorName) + "\",";
  json += "\"signal\":\"" + jsonEscape(signal) + "\",";
  json += "\"rssi\":" + String(_modem.rssi()) + ",";
  json += "\"handset\":\"" + String(_phoneLine.isOffHook() ? "OFF HOOK" : "ON HOOK") + "\",";
  json += "\"phoneState\":\"" + jsonEscape(_phoneState) + "\",";
  json += "\"modem\":\"" + String(_modem.isOnline() ? "ONLINE" : "NO RESPONSE") + "\",";
  json += "\"powerSaving\":" + String(_hardwareControl.powerSavingEnabled() ? "true" : "false") + ",";
  json += "\"ag1171PowerState\":\"" + String(_hardwareControl.ag1171PowerStateName()) + "\",";
  json += "\"developmentPowerLoggingEnabled\":" +
          String(ENABLE_DEVELOPMENT_POWER_LOGGING ? "true" : "false") + ",";
  String developmentPowerStatus = "Disabled in Config.h";
  if (ENABLE_DEVELOPMENT_POWER_LOGGING) {
    if (_powerLogger.isReady() && _powerLogger.hasSample()) {
      developmentPowerStatus = String(_powerLogger.filename()) + ": " +
        String(_powerLogger.latestBusVoltageV(), 3) + " V, " +
        String(_powerLogger.latestCurrentA(), 3) + " A, " +
        String(_powerLogger.latestPowerW(), 3) + " W";
    } else if (_powerLogger.isReady()) {
      developmentPowerStatus = String(_powerLogger.filename()) + ": waiting for first sample";
    } else {
      developmentPowerStatus = "Enabled, but INA226 or microSD is unavailable";
    }
  }
  json += "\"developmentPowerStatus\":\"" + jsonEscape(developmentPowerStatus) + "\",";
  json += "\"clock\":\"" + jsonEscape(_clock.displayTime()) + "\",";
  json += "\"clockSynced\":" + String(_clock.isSynced() ? "true" : "false") + ",";
  json += "\"clockSource\":\"" + jsonEscape(_clock.sourceName()) + "\",";
  json += "\"wifiSecondsRemaining\":" + String(secondsRemaining());
  json += "}";
  return json;
}

void MaintenanceService::handleRoot() {
  // Treat a page load/reload as a fresh browser console session.
  _console.resetWebSession();
  _server.send_P(200, "text/html", PAGE_HTML);
}
void MaintenanceService::handleStatus() { _server.send(200, "application/json", statusJson()); }

void MaintenanceService::handleLog() {
  uint64_t since = 0;
  if (_server.hasArg("since")) since = strtoull(_server.arg("since").c_str(), nullptr, 10);
  uint64_t start = 0, end = 0;
  String data = LogSerial.readFrom(since, WEB_LOG_CHUNK_BYTES, start, end);
  char startBuffer[24];
  char endBuffer[24];
  snprintf(startBuffer, sizeof(startBuffer), "%llu", static_cast<unsigned long long>(start));
  snprintf(endBuffer, sizeof(endBuffer), "%llu", static_cast<unsigned long long>(end));
  _server.sendHeader("X-Log-Start", String(startBuffer));
  _server.sendHeader("X-Log-End", String(endBuffer));
  _server.send(200, "text/plain; charset=utf-8", data);
}

void MaintenanceService::handleEvents() { _server.send(200, "text/plain; charset=utf-8", _eventLog.contents()); }

void MaintenanceService::handleCommand() {
  String command = _server.arg("plain");
  command.trim();
  if (command.length()) {
    LogSerial.print("[WEB>] "); LogSerial.println(command);
    _console.submitWebLine(command);
  }
  _server.send(204, "text/plain", "");
}

void MaintenanceService::handleTimeSync() {
  String body = _server.arg("plain");
  body.trim();
  const int comma = body.indexOf(',');
  if (comma < 1) {
    _server.send(400, "text/plain", "Expected epochMilliseconds,browserTimezoneOffsetMinutes");
    return;
  }

  const uint64_t epochMs = strtoull(body.substring(0, comma).c_str(), nullptr, 10);
  const int browserOffsetMinutes = body.substring(comma + 1).toInt();
  const time_t utcEpoch = static_cast<time_t>(epochMs / 1000ULL);

  // Browser getTimezoneOffset() is UTC - local; our clock stores local - UTC.
  const int localOffsetMinutes = -browserOffsetMinutes;

  struct tm tmValue;
  gmtime_r(&utcEpoch, &tmValue);
  const int year = tmValue.tm_year + 1900;
  if (year < 2024 || year > 2069 || localOffsetMinutes < -14 * 60 || localOffsetMinutes > 14 * 60) {
    _server.send(400, "text/plain", "Browser time failed sanity check");
    return;
  }

  const bool firstBrowserSync = !_clock.isSynced() || _clock.sourceName() != "BROWSER";
  _clock.setBrowserTime(utcEpoch, localOffsetMinutes);
  _modem.setClock(utcEpoch, localOffsetMinutes);

  LogSerial.print("CLOCK: synchronized from browser: ");
  LogSerial.println(_clock.displayTime());
  if (firstBrowserSync) _eventLog.add("Clock synchronized from maintenance browser");
  _server.send(204, "text/plain", "");
}

void MaintenanceService::handleClearRamLog() {
  LogSerial.clearHistory();
  LogSerial.println("LOG: RAM console history cleared from web interface");
  _server.send(204, "text/plain", "");
}

void MaintenanceService::handleClearEvents() {
  _eventLog.clear();
  _eventLog.add("Persistent event log cleared");
  _server.send(204, "text/plain", "");
}

void MaintenanceService::handleWifiOff() {
  _server.send(200, "text/plain", "Wi-Fi shutting down");
  delay(50);
  disable();
}

void MaintenanceService::handlePowerSavingToggle() {
  const bool enabled = !_hardwareControl.powerSavingEnabled();
  _hardwareControl.setPowerSavingEnabled(enabled);
  _eventLog.add(String("Experimental AG1171 power saving ") +
                (enabled ? "enabled" : "disabled"));
  _server.send(200, "application/json", statusJson());
}


void MaintenanceService::handleOtaUpload() {
  HTTPUpload& upload = _server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    _enabledAt = millis();
    _otaUploadOk = false;
    LogSerial.print("OTA: upload starting: ");
    LogSerial.println(upload.filename);
    _eventLog.add("OTA firmware upload started: " + upload.filename);
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      LogSerial.print("OTA: Update.begin failed, error ");
      LogSerial.println(Update.getError());
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    _enabledAt = millis();
    if (!Update.hasError()) {
      const size_t written = Update.write(upload.buf, upload.currentSize);
      if (written != upload.currentSize) {
        LogSerial.print("OTA: write failed, error ");
        LogSerial.println(Update.getError());
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (!Update.hasError() && Update.end(true)) {
      _otaUploadOk = true;
      LogSerial.print("OTA: image accepted, bytes = ");
      LogSerial.println(upload.totalSize);
      _eventLog.add("OTA firmware update accepted: " + String(upload.totalSize) + " bytes; rebooting");
    } else {
      LogSerial.print("OTA: update failed, error ");
      LogSerial.println(Update.getError());
      _eventLog.add("OTA firmware update FAILED; Update error " + String(Update.getError()));
    }
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    Update.abort();
    LogSerial.println("OTA: upload aborted");
    _eventLog.add("OTA firmware upload aborted");
  }
}

void MaintenanceService::handleOtaComplete() {
  if (_otaUploadOk) {
    if (!requestWifiAfterOta()) {
      LogSerial.println("OTA: WARNING - could not save post-update Wi-Fi request");
      _eventLog.add("OTA warning: could not request maintenance Wi-Fi after reboot");
    }
    _server.send(200, "text/html", "<html><body><h2>Update successful</h2><p>ESP-RotaryCell is rebooting into the new firmware.</p></body></html>");
    delay(500);
    ESP.restart();
  } else {
    _server.send(500, "text/plain", "Firmware update failed. Check the live/persistent log before retrying.");
  }
}

namespace {
bool validPowerName(const String& name) {
  if (name.length() != 12 || !name.startsWith("POWER") || !name.endsWith(".CSV")) return false;
  for (int i=5; i<8; ++i) if (name[i] < '0' || name[i] > '9') return false;
  return true;
}
}
void MaintenanceService::handlePowerFiles() {
  if (!_powerLogger.sdReady()) { _server.send(503, "text/plain", "SD logging storage unavailable (check card and Config.h)."); return; }
  File root = SD.open("/");
  if (!root) { _server.send(503, "text/plain", "Cannot read SD card"); return; }
  String json = "[";
  File file = root.openNextFile();
  while (file) {
    String name = file.name();
    if (name.startsWith("/")) name.remove(0,1);
    if (!file.isDirectory() && validPowerName(name)) {
      if (json.length()>1) json += ",";
      json += "{\"name\":\"" + name + "\",\"size\":" + String(file.size()) + "}";
    }
    file.close(); file = root.openNextFile();
  }
  root.close(); json += "]";
  _server.send(200, "application/json", json);
}
void MaintenanceService::handlePowerDownload() {
  const String name = _server.arg("name");
  if (!validPowerName(name)) {
    _server.send(400,"text/plain","Invalid file request"); return;
  }
  if (!_powerLogger.sdReady() || !_powerLogger.flushForDownload()) {
    _server.send(503,"text/plain","SD card unavailable or flush failed"); return;
  }
  File file = SD.open("/" + name, FILE_READ);
  if (!file || file.isDirectory()) { _server.send(404,"text/plain","Log file not found"); return; }
  _enabledAt = millis();
  _server.sendHeader("Content-Disposition", "attachment; filename=" + name);
  _server.sendHeader("Cache-Control", "no-store");
  _server.sendHeader("Connection", "close");
  _server.streamFile(file, "text/csv");
  file.close();
  _enabledAt = millis();
}
