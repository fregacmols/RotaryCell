#include "MaintenanceService.h"
#include "Usability.h"
#include "HardwareControl.h"
#include "ClockManager.h"
#include "Modem.h"

namespace {
bool integer(const String& text, unsigned lo,unsigned hi,unsigned& value) {
  if(!text.length()) return false;
  value=0;
  for(char c:text) { if(c<'0'||c>'9') return false; value=value*10+c-'0'; if(value>hi) return false; }
  return value>=lo;
}
bool validList(const String& text,String& normalized) {
  if(text.length()>1200) return false;
  int start=0; unsigned count=0;
  while(start<text.length()) {
    int end=text.indexOf('\n',start); if(end<0) end=text.length();
    String entry=text.substring(start,end); entry.trim();
    if(entry.length()) {
      for(char c:entry) if(!((c>='0'&&c<='9')||c=='+'||c==' '||c=='-'||c=='('||c==')'||c=='.')) return false;
      String number=Usability::normalize(entry);
      if(!number.length()||number.length()>20||++count>50) return false;
      if ((String("\n")+normalized+"\n").indexOf(String("\n")+number+"\n")<0) {
        if(normalized.length()) normalized+='\n'; normalized+=number;
      }
    }
    start=end+1;
  }
  return normalized.length()<=1200;
}
bool timeMinutes(const String& text,uint16_t& value) {
  if(text.length()!=5||text[2]!=':') return false;
  unsigned hour,minute;
  if(!integer(text.substring(0,2),0,23,hour)||!integer(text.substring(3),0,59,minute)) return false;
  value=hour*60+minute; return true;
}
String timeText(uint16_t minutes) { char b[6]; snprintf(b,sizeof(b),"%02u:%02u",minutes/60,minutes%60); return b; }
}

void MaintenanceService::configureUsabilityRoutes() {
  _server.on("/api/settings",HTTP_GET,[this]() {
    const auto& s=usability.settings;
    String json="{";
    auto text=[&](const char* key,const String& value) { json+=Usability::quote(key)+":"+Usability::quote(value)+","; };
    auto number=[&](const char* key,unsigned value) { json+=Usability::quote(key)+":"+String(value)+","; };
    text("ssid",s.ssid); text("incomingList",s.incomingList); text("outgoingList",s.outgoingList);
    number("incomingMode",s.incomingMode);number("outgoingMode",s.outgoingMode);
    number("privatePolicy",s.privatePolicy);number("unknownPolicy",s.unknownPolicy);
    number("block911",s.block911);number("blockPremium",s.blockPremium);number("rejectShort",s.rejectShort);number("minimumDigits",s.minimumDigits);
    number("scheduledDnd",s.scheduledDnd);number("manualDnd",s.manualDnd);
    text("dndStart",timeText(s.dndStart));text("dndEnd",timeText(s.dndEnd));text("timezone",s.timezone);
    number("powerSaving",s.powerSaving);number("pollMs",s.pollMs);
    number("dndActive",usability.dndActive());number("clockValid",_clock.isSynced());number("historyReady",usability.historyReady());
    json.remove(json.length()-1);json+='}'; _server.send(200,"application/json",json);
  });
  _server.on("/api/settings",HTTP_POST,[this]() {
    UserSettings s=usability.settings; String section=_server.arg("section");
    bool valid=true;unsigned value;
    auto num=[&](const char* key,unsigned lo,unsigned hi) { if(!integer(_server.arg(key),lo,hi,value)) {valid=false;return lo;} return value; };
    auto flag=[&](const char* key) { return num(key,0,1)==1; };
    if(section=="calls") {
      s.incomingMode=num("incomingMode",0,2);s.outgoingMode=num("outgoingMode",0,2);
      s.privatePolicy=num("privatePolicy",0,2);s.unknownPolicy=num("unknownPolicy",0,2);
      s.block911=flag("block911");s.blockPremium=flag("blockPremium");s.rejectShort=flag("rejectShort");s.minimumDigits=num("minimumDigits",1,20);
      String incoming,outgoing;
      valid=validList(_server.arg("incomingList"),incoming)&&validList(_server.arg("outgoingList"),outgoing)&&valid;
      strlcpy(s.incomingList,incoming.c_str(),sizeof(s.incomingList));strlcpy(s.outgoingList,outgoing.c_str(),sizeof(s.outgoingList));
    } else if(section=="phone") {
      s.scheduledDnd=flag("scheduledDnd");s.manualDnd=flag("manualDnd");
      valid=timeMinutes(_server.arg("dndStart"),s.dndStart)&&timeMinutes(_server.arg("dndEnd"),s.dndEnd)&&valid;
      if(s.scheduledDnd && s.dndStart==s.dndEnd) valid=false;
      String zone=_server.arg("timezone");
      const char* zones[]={"UTC0","EST5EDT,M3.2.0/2,M11.1.0/2","CST6CDT,M3.2.0/2,M11.1.0/2","MST7MDT,M3.2.0/2,M11.1.0/2","MST7","PST8PDT,M3.2.0/2,M11.1.0/2","AKST9AKDT,M3.2.0/2,M11.1.0/2","HST10"};
      bool found=false;for(auto z:zones) if(zone==z) found=true;
      valid=valid&&found;strlcpy(s.timezone,zone.c_str(),sizeof(s.timezone));
    } else if(section=="power") {
      s.powerSaving=flag("powerSaving");s.pollMs=num("pollMs",500,2000);
      valid=valid&&(s.pollMs==500||s.pollMs==1000||s.pollMs==2000);
    } else if(section=="wifi") {
      String ssid=_server.arg("ssid"),password=_server.arg("password");
      valid=ssid.length()>0&&ssid.length()<=32;
      for(char c:ssid) if(static_cast<uint8_t>(c)<32) valid=false;
      strlcpy(s.ssid,ssid.c_str(),sizeof(s.ssid));
      if(password.length()) {
        valid=valid&&password.length()>=8&&password.length()<=63;
        for(char c:password) if(static_cast<uint8_t>(c)<32||static_cast<uint8_t>(c)>126) valid=false;
        strlcpy(s.password,password.c_str(),sizeof(s.password));
      }
    } else valid=false;
    if(!valid) {_server.send(400,"text/plain","Invalid settings. Lists allow up to 50 numbers. Scheduled start/end must differ.");return;}
    if((_modem.isCallActive() || _modem.isIncomingCall())) {_server.send(409,"text/plain","Finish the call before changing settings.");return;}
    if(!usability.save(s)) {_server.send(500,"text/plain","Unable to save settings; previous settings retained.");return;}
    _hardwareControl.setPowerSavingEnabled(s.powerSaving);_hardwareControl.setPollInterval(s.pollMs);
    _server.send(200,"text/plain",section=="wifi"?"Saved. Apply Wi-Fi settings to reconnect using the new credentials.":"Saved");
  });
  _server.on("/api/history",HTTP_GET,[this]() {_server.send(200,"application/json",usability.historyJson());});
  _server.on("/api/history/clear",HTTP_POST,[this]() {
    bool ok=usability.clearHistory();_server.send(ok?200:500,"text/plain",ok?"History cleared; number lists retained.":"Unable to clear history");
  });
  _server.on("/api/wifi/apply",HTTP_POST,[this]() {
    if((_modem.isCallActive() || _modem.isIncomingCall())) {_server.send(409,"text/plain","Finish the call first.");return;}
    _server.send(200,"text/plain","Applying Wi-Fi settings. Reconnect to the saved network.");_wifiApplyAt=millis();
  });
}
