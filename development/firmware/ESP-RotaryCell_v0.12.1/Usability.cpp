#include "Usability.h"
#include "CallPolicy.h"
#include <LittleFS.h>
#include <time.h>
#include "LogSerial.h"
Usability usability;
namespace {
uint32_t checksum(const CallRecord& r) {
  const uint8_t* p = reinterpret_cast<const uint8_t*>(&r);
  uint32_t h = 2166136261UL;
  for (size_t i=0; i<offsetof(CallRecord, checksum); ++i) h = (h ^ p[i]) * 16777619UL;
  return h;
}
String path(unsigned slot) { return "/calls/" + String(slot) + ".bin"; }
bool readPath(const String& filename, CallRecord& r) {
  File f = LittleFS.open(filename, "r");
  if (!f || f.size()!=sizeof(r)) return false;
  if (f.read(reinterpret_cast<uint8_t*>(&r), sizeof(r))!=sizeof(r)) return false;
  return r.sequence && r.checksum==checksum(r) && r.number[31]==0 && r.identity[15]==0 && r.outcome[39]==0;
}
bool readRecord(unsigned slot, CallRecord& r) { return readPath(path(slot),r); }
bool listed(const char* list, const String& number) {
  String entries(list); int start=0;
  while (start < entries.length()) {
    int end=entries.indexOf('\n',start); if(end<0) end=entries.length();
    if (Usability::normalize(entries.substring(start,end))==number) return true;
    start=end+1;
  }
  return false;
}
}
void Usability::begin(ClockManager& clock) {
  _clock=&clock;
  _prefsReady=_prefs.begin("usability", false);
  if (_prefsReady && _prefs.getBytesLength("settings")==sizeof(UserSettings)) {
    UserSettings loaded;
    _prefs.getBytes("settings", &loaded, sizeof(loaded));
    if (loaded.version==1 && loaded.ssid[32]==0 && loaded.password[63]==0 &&
        loaded.incomingList[1200]==0 && loaded.outgoingList[1200]==0 && loaded.timezone[64]==0)
      settings=loaded;
  }
  setenv("TZ", settings.timezone, 1); tzset();
  _historyReady=LittleFS.exists("/calls") || LittleFS.mkdir("/calls");
  if (_historyReady) {
    for(unsigned i=0;i<100;++i) { CallRecord r{}; if(readRecord(i,r)) _sequence=max(_sequence,r.sequence); }
    CallRecord pending{};
    if(readPath("/calls/pending.tmp",pending) && pending.sequence>_sequence &&
       LittleFS.rename("/calls/pending.tmp",path((pending.sequence-1)%100))) _sequence=pending.sequence;
  }
}
bool Usability::save(const UserSettings& candidate) {
  if (!_prefsReady || _prefs.putBytes("settings", &candidate, sizeof(candidate))!=sizeof(candidate)) return false;
  settings=candidate; setenv("TZ",settings.timezone,1); tzset(); return true;
}
bool Usability::resetWifi() {
  UserSettings candidate=settings;
  strlcpy(candidate.ssid,"ESP-RotaryCell",sizeof(candidate.ssid));
  strlcpy(candidate.password,"rotarycell",sizeof(candidate.password));
  return save(candidate);
}
bool Usability::dndActive() const {
  if (settings.manualDnd) return true;
  if (!settings.scheduledDnd || !_clock || !_clock->isSynced()) return false;
  time_t utc=_clock->currentUtc(); struct tm local{}; localtime_r(&utc,&local);
  int minute=local.tm_hour*60+local.tm_min;
  return CallPolicy::dailyWindow(minute,settings.dndStart,settings.dndEnd);
}
String Usability::normalize(const String& number) {
  String result;
  for(char c:number) if(c>='0' && c<='9') result+=c;
  // North American +1/1 and ten-digit representations identify the same line.
  if(result.length()==11 && result[0]=='1') result.remove(0,1);
  return result;
}
bool Usability::allowed(bool incoming,const String& number,const String& identity,String& reason) const {
  String normalized=normalize(number);
  uint8_t mode=incoming?settings.incomingMode:settings.outgoingMode;
  if(incoming && identity!="number") {
    uint8_t policy=identity=="private"?settings.privatePolicy:settings.unknownPolicy;
    bool allow=CallPolicy::unidentifiedAllowed(mode,policy);
    if(!allow) reason=identity+" caller ID";
    return allow;
  }
  if(!incoming) {
    if(settings.block911 && normalized=="911") { reason="911 restriction"; return false; }
    if(settings.blockPremium && (normalized.startsWith("900") || normalized.startsWith("1900"))) { reason="premium-rate restriction"; return false; }
    if(settings.rejectShort && normalized.length()<settings.minimumDigits) { reason="short-number restriction"; return false; }
  }
  if(mode==0) return true;
  bool match=listed(incoming?settings.incomingList:settings.outgoingList,normalized);
  bool allow=CallPolicy::listedAllowed(mode,match);
  if(!allow) reason=mode==1?"not on whitelist":"on blacklist";
  return allow;
}
void Usability::startCall(bool incoming,const String& number,const String& identity) {
  if(_pending) finishCall("interrupted");
  memset(&_current,0,sizeof(_current)); _pending=true;
  _current.incoming=incoming; _current.utc=_clock->isSynced()?_clock->currentUtc():0;
  _current.silenced=incoming && dndActive(); identify(number,identity);
}
void Usability::identify(const String& number,const String& identity) {
  if(!_pending) return;
  strlcpy(_current.number,number.c_str(),sizeof(_current.number));
  strlcpy(_current.identity,identity.c_str(),sizeof(_current.identity));
}
void Usability::markConnected() { if(_pending) _current.connected=true; }
void Usability::finishCall(const String& outcome) {
  if(!_pending) return;
  _pending=false; _current.sequence=++_sequence;
  strlcpy(_current.outcome,outcome.c_str(),sizeof(_current.outcome));
  _current.checksum=checksum(_current);
  if(!_historyReady) { LogSerial.println("CALL HISTORY: storage unavailable"); return; }
  File f=LittleFS.open("/calls/pending.tmp","w");
  if(!f) return;
  bool ok=f.write(reinterpret_cast<const uint8_t*>(&_current),sizeof(_current))==sizeof(_current);
  f.flush(); f.close();
  if(ok) ok=LittleFS.rename("/calls/pending.tmp",path((_sequence-1)%100));
  if(!ok) LogSerial.println("CALL HISTORY: save failed");
}
String Usability::quote(const String& value) {
  String s="\"";
  for(char c:value) { if(c=='"'||c=='\\') { s+='\\';s+=c; } else if(c=='\n') s+="\\n"; else if(c=='\r') s+="\\r"; else if(static_cast<uint8_t>(c)>=32) s+=c; }
  return s+'"';
}
String Usability::historyJson() const {
  String json="["; bool first=true;
  for(unsigned n=0;n<100 && n<_sequence;++n) {
    uint32_t seq=_sequence-n; CallRecord r{};
    if(!readRecord((seq-1)%100,r) || r.sequence!=seq) continue;
    if(!first) json+=','; first=false;
    String localTime="Time unavailable";
    if(r.utc) {
      time_t utc=static_cast<time_t>(r.utc); struct tm local{};localtime_r(&utc,&local);
      char text[40];strftime(text,sizeof(text),"%Y-%m-%d %H:%M:%S %Z",&local);localTime=text;
    }
    json+="{\"number\":"+quote(r.number)+",\"identity\":"+quote(r.identity)+",\"outcome\":"+quote(r.outcome)+",\"incoming\":"+String(r.incoming?"true":"false")+",\"connected\":"+String(r.connected?"true":"false")+",\"silenced\":"+String(r.silenced?"true":"false")+",\"localTime\":"+quote(localTime)+",\"utc\":"+String(static_cast<long long>(r.utc))+"}";
  }
  return json+"]";
}
bool Usability::clearHistory() {
  if(!_historyReady) return false;
  bool ok=true;
  for(unsigned i=0;i<100;++i) if(LittleFS.exists(path(i)) && !LittleFS.remove(path(i))) ok=false;
  if(LittleFS.exists("/calls/pending.tmp")) LittleFS.remove("/calls/pending.tmp");
  if(ok) _sequence=0; return ok;
}
