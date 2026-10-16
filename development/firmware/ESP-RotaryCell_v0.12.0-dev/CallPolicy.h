#pragma once
#include <stdint.h>

namespace CallPolicy {
constexpr bool unidentifiedAllowed(uint8_t mode, uint8_t policy) {
  return policy==1 || (policy==0 && mode!=1);
}
constexpr bool listedAllowed(uint8_t mode, bool listed) {
  return mode==0 || (mode==1 ? listed : !listed);
}
constexpr bool dailyWindow(unsigned minute,unsigned start,unsigned end) {
  return start==end ? false : start<end ? minute>=start && minute<end : minute>=start || minute<end;
}
// CLI validity is the sixth CSV field. Ignore commas inside quoted alpha text.
constexpr int cliValidity(const char* line) {
  unsigned i=0;
  while(line[i] && line[i]!=':') ++i;
  if(!line[i]) return -1;
  ++i; unsigned field=0; bool quoted=false; int value=-1;
  for(;line[i];++i) {
    char c=line[i];
    if(c=='"') { quoted=!quoted; continue; }
    if(c==',' && !quoted) { if(field==5) return value; ++field; continue; }
    if(field==5 && c!=' ' && c!='\r' && c!='\n') {
      if(c<'0'||c>'2'||value!=-1) return -1;
      value=c-'0';
    }
  }
  return field==5 ? value : -1;
}
}
