#include "MaintenanceService.h"
#include "Config.h"
#include "Usability.h"
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
body{font-family:system-ui,sans-serif;margin:0;background:#111;color:#eee}main{max-width:1000px;margin:auto;padding:18px}.card{background:#1d1d1d;border:1px solid #333;border-radius:10px;padding:14px;margin-bottom:14px}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:8px}.metric{background:#282828;border-radius:7px;padding:10px}.k{color:#aaa;font-size:.8rem}.v{font-size:1.08rem;margin-top:3px}pre{background:#050505;color:#e7e7e7;padding:12px;border-radius:7px;height:42vh;overflow:auto;white-space:pre-wrap;word-break:break-word}input{width:calc(100% - 110px);padding:9px;background:#090909;color:#fff;border:1px solid #555;border-radius:5px}button{padding:9px 12px;margin:3px;border:0;border-radius:5px;cursor:pointer}.danger{background:#8b2525;color:#fff}.small{color:#aaa;font-size:.85rem}h1{margin-top:0}nav{display:flex;flex-wrap:wrap;gap:4px;margin-bottom:16px}nav button[aria-current="page"]{background:#91c9ff}label{display:block;margin:12px 0}label input,label select,textarea{display:block;width:100%;box-sizing:border-box;margin-top:5px;padding:9px;background:#090909;color:#fff;border:1px solid #555;border-radius:5px}.check input{display:inline;width:auto;margin-right:8px}textarea{min-height:110px}table{width:100%;border-collapse:collapse}td,th{text-align:left;padding:8px;border-bottom:1px solid #444}.history-scroll{overflow:auto}.feedback{color:#b4d8ff}[hidden]{display:none!important}</style></head>
<body><main><h1 id="heading">RotaryCell</h1><nav aria-label="Maintenance sections"><button type="button" data-tab="overview">Overview</button><button type="button" data-tab="calls">Calls</button><button type="button" data-tab="phone">Phone Settings</button><button type="button" data-tab="power">Power Saving</button><button type="button" data-tab="diagnostics">Diagnostics</button><button type="button" data-tab="system">System</button></nav><p id="dndSummary" role="status"></p><section id="tab-overview" ><div class="card"><div class="grid" id="status"></div><div class="small" id="countdown"></div><button id="wifiTimer">Keep Wi-Fi on</button></div></section>
<section id="tab-calls" hidden><form class="settings card" data-section="calls"><h2>Call filtering</h2><label>Incoming calls<select name="incomingMode"><option value="0">Off</option><option value="1">Whitelist — allow listed numbers</option><option value="2">Blacklist — block listed numbers</option></select></label>
<label>Withheld/private caller ID<select name="privatePolicy"><option value="0">Follow filter mode</option><option value="1">Allow</option><option value="2">Block</option></select></label>
<label>Unavailable/unknown caller ID<select name="unknownPolicy"><option value="0">Follow filter mode</option><option value="1">Allow</option><option value="2">Block</option></select></label>
<label>Incoming list — one number per line<textarea name="incomingList" maxlength="1200"></textarea></label>
<label>Outgoing calls<select name="outgoingMode"><option value="0">Off</option><option value="1">Whitelist — allow listed numbers</option><option value="2">Blacklist — block listed numbers</option></select></label>
<label>Outgoing list — one number per line<textarea name="outgoingList" maxlength="1200"></textarea></label>
<label class="check"><input type="checkbox" name="block911">Block 911</label>
<label class="check"><input type="checkbox" name="blockPremium">Block North American 900/1-900 numbers</label>
<label class="check"><input type="checkbox" name="rejectShort">Reject short numbers</label>
<label>Minimum digits<input name="minimumDigits" type="number" min="1" max="20"></label><p class="small">Up to 50 numbers per direction. +1 and ten-digit North American numbers match. Service codes remain available. Restrictions apply in addition to lists.</p><button>Save settings</button><p class="feedback" role="status"></p></form><div class="card"><h2>Call history</h2><p class="small">Latest 100 calls, saved across restarts. Select numbers to add them to the current filter list. Save the filtering form to keep additions.</p><button id="refreshHistory">Refresh</button><button id="clearHistory">Clear History</button><div id="history"></div></div></section>
<section id="tab-phone" hidden><form class="settings card" data-section="phone"><h2>Do Not Disturb</h2><label class="check"><input type="checkbox" name="manualDnd">On until turned off</label>
<label class="check"><input type="checkbox" name="scheduledDnd">Enable daily schedule</label>
<label>Start<input name="dndStart" type="time" required></label>
<label>End<input name="dndEnd" type="time" required></label>
<label>Phone time zone<select name="timezone"><option value="UTC0">UTC</option><option value="EST5EDT,M3.2.0/2,M11.1.0/2">US Eastern</option><option value="CST6CDT,M3.2.0/2,M11.1.0/2">US Central</option><option value="MST7MDT,M3.2.0/2,M11.1.0/2">US Mountain</option><option value="MST7">Arizona</option><option value="PST8PDT,M3.2.0/2,M11.1.0/2">US Pacific</option><option value="AKST9AKDT,M3.2.0/2,M11.1.0/2">Alaska</option><option value="HST10">Hawaii</option></select></label><p class="small">Silences the bell; calls can still be answered. Overnight schedules are supported. Without a valid clock, scheduled DND pauses. Dial 0002 to toggle manual DND; turning it off returns to the schedule.</p><button>Save settings</button><p class="feedback" role="status"></p></form></section>
<section id="tab-power" hidden><form class="settings card" data-section="power"><h2>Power Saving</h2><label>AG1171 behavior<select name="powerSaving"><option value="1">Idle power saving (default)</option><option value="0">Always on — fastest response</option></select></label>
<label>Idle sleep interval<select name="pollMs"><option value="500">500 ms (default)</option><option value="1000">1 second</option><option value="2000">2 seconds</option></select></label><p class="small">Longer sleep intervals can delay handset pickup detection. Incoming calls wake the interface immediately; it stays awake during calls.</p><button>Save settings</button><p class="feedback" role="status"></p></form><button id="powerSaving" hidden></button></section>
<section id="tab-diagnostics" hidden><div class="card" id="devPower" style="display:none"><h2>Development power logger</h2><p id="devPowerStatus"></p><p class="small">INA226 measurements are recorded at 10 Hz to the LilyGO microSD card. This panel and the instrumentation are controlled by ENABLE_DEVELOPMENT_POWER_LOGGING in Config.h.</p></div>
<div class="card"><h2>Live console</h2><pre id="console"></pre><form id="cmd"><input id="input" autocomplete="off" placeholder="S, M, A, AT command, EXIT..."><button>Send</button></form><button id="clearRam">Clear RAM history</button></div>
<div class="card"><h2>Persistent event log</h2><pre id="events" style="height:24vh"></pre><a href="/download/events"><button>Download event log</button></a><button id="refreshEvents">Refresh events</button><button id="clearEvents">Clear events</button></div>
<div class="card"><h2>SD power logs</h2><button id="refreshFiles">Refresh file list</button><div id="powerFiles"></div><p id="downloadProgress" class="small"></p></div></section>
<section id="tab-system" hidden><form class="settings card" data-section="wifi"><h2>Maintenance Wi-Fi</h2><label>Network name<input name="ssid" maxlength="32" required></label>
<label>New password<input name="password" type="password" minlength="8" maxlength="63" autocomplete="new-password" placeholder="Leave blank to keep current password"></label><button type="button" id="showPassword">Show password</button><p class="small">Password must be 8–63 printable characters. Save first, then apply. Applying disconnects this browser. Dial 0001 twice within 30 seconds to restore default credentials; hanging up cancels confirmation.</p><button>Save settings</button><p class="feedback" role="status"></p></form><button id="applyWifi">Apply saved Wi-Fi settings</button><p id="wifiFeedback" role="status"></p><button class="danger" id="wifiOff">Turn Wi-Fi off</button><div class="card"><h2>Handset service codes</h2><p>0000: toggle maintenance Wi-Fi. 0001 twice within 30 seconds: restore Wi-Fi credentials. 0002: toggle manual Do Not Disturb. 9999: diagnostic hardware power cycle.</p><p class="small">For a full 9999 power cycle, disconnect USB and operate from the battery. Hanging up cancels an armed credential reset.</p></div><div class="card"><h2>Firmware update</h2><p class="small">Upload the application .bin exported by Arduino IDE. The phone will reboot automatically after a successful update.</p><form method="POST" action="/update" enctype="multipart/form-data"><input type="file" name="firmware" accept=".bin,application/octet-stream" required><button type="submit">Install firmware</button></form></div></section>
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

let settingsLoaded=false;
const forms=[...document.querySelectorAll('form.settings')];
document.querySelectorAll('[data-tab]').forEach(b=>b.onclick=()=>{
 document.querySelectorAll('section[id^="tab-"]').forEach(p=>p.hidden=p.id!=='tab-'+b.dataset.tab);
 document.querySelectorAll('[data-tab]').forEach(n=>n.setAttribute('aria-current',n===b?'page':'false'));
 if(b.dataset.tab==='calls') loadHistory();
});
document.querySelector('[data-tab="overview"]').setAttribute('aria-current','page');
async function summary(){try{const s=await(await fetch('/api/settings')).json();
 document.getElementById('dndSummary').textContent=(s.dndActive?('Do Not Disturb active — bell silenced · '+(s.manualDnd?'until turned off':'until '+s.dndEnd)):'Do Not Disturb inactive')+(!s.clockValid&&s.scheduledDnd?' · Schedule paused: waiting for clock':'')+(!s.historyReady?' · Call history storage unavailable':'');
 for(const f of forms)if(!f.dirty)for(const e of f.elements){if(!e.name||e.name==='password')continue;if(e.type==='checkbox')e.checked=!!s[e.name];else e.value=s[e.name];}settingsLoaded=true;
}catch(e){document.getElementById('dndSummary').textContent='Unable to read phone settings';}}
forms.forEach(f=>{f.oninput=()=>{f.dirty=true;};f.onsubmit=async e=>{e.preventDefault();const body=new URLSearchParams();body.set('section',f.dataset.section);
 for(const i of f.elements)if(i.name)body.set(i.name,i.type==='checkbox'?(i.checked?'1':'0'):i.value);
 try{let r=await fetch('/api/settings',{method:'POST',body});const msg=await r.text();f.querySelector('.feedback').textContent=msg;if(r.ok){f.dirty=false;if(f.dataset.section==='wifi')f.elements.password.value='';summary();}}catch(e){f.querySelector('.feedback').textContent='Connection lost; check settings after reconnecting.';}
};});
document.getElementById('showPassword').onclick=()=>{const i=document.querySelector('[name="password"]');i.type=i.type==='password'?'text':'password';document.getElementById('showPassword').textContent=i.type==='password'?'Show password':'Hide password';};
document.getElementById('applyWifi').onclick=async()=>{if(!confirm('Apply saved Wi-Fi settings? This will disconnect you.'))return;try{const r=await fetch('/api/wifi/apply',{method:'POST'});document.getElementById('wifiFeedback').textContent=await r.text();}catch(e){document.getElementById('wifiFeedback').textContent='Reconnect using the saved credentials.';}};
async function loadHistory(){const panel=document.getElementById('history');try{const records=await(await fetch('/api/history')).json();panel.replaceChildren();if(!records.length){panel.textContent='No calls recorded.';return;}
 const wrap=document.createElement('div');wrap.className='history-scroll';const table=document.createElement('table');wrap.append(table);panel.append(wrap);
 const head=document.createElement('tr');for(const name of ['Direction','Number','Time','Outcome','Add to list']){const th=document.createElement('th');th.textContent=name;head.append(th);}table.append(head);
 for(const r of records){const row=document.createElement('tr');const outcome=(r.connected?'Connected · ':'')+r.outcome+(r.silenced?' · DND':'');for(const value of [r.incoming?'Incoming':'Outgoing',r.number||r.identity,r.utc?(r.localTime||new Date(r.utc*1000).toISOString().replace('T',' ').replace('.000Z',' UTC')):'Time unavailable',outcome]){const td=document.createElement('td');td.textContent=value;row.append(td);}const td=document.createElement('td');if(r.number){const b=document.createElement('button');b.textContent=r.incoming?'Incoming list':'Outgoing list';b.onclick=()=>{const input=document.querySelector('[name="'+(r.incoming?'incomingList':'outgoingList')+'"]');const items=input.value.split('\n').filter(Boolean);if(!items.includes(r.number))items.push(r.number);input.value=items.join('\n');forms.find(f=>f.dataset.section==='calls').dirty=true;b.textContent='Added — save above';};td.append(b);}row.append(td);table.append(row);}
}catch(e){panel.textContent='Unable to load call history.';}}
document.getElementById('refreshHistory').onclick=loadHistory;
document.getElementById('clearHistory').onclick=async()=>{if(confirm('Clear call history? Filter lists will remain.')){let r=await fetch('/api/history/clear',{method:'POST'});if(!r.ok)alert(await r.text());loadHistory();}};
summary();setInterval(summary,5000);

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
  configureUsabilityRoutes();
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
  if (!WiFi.softAP(usability.settings.ssid, usability.settings.password)) {
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
  LogSerial.print("WIFI: SSID = "); LogSerial.println(usability.settings.ssid);
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
  if(_wifiApplyAt && millis()-_wifiApplyAt>=750) { _wifiApplyAt=0; disable(); enable(); return; }
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
  UserSettings candidate=usability.settings;
  const bool enabled = !candidate.powerSaving;
  candidate.powerSaving=enabled;
  if(!usability.save(candidate)) { _server.send(500,"text/plain","Save failed"); return; }
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
