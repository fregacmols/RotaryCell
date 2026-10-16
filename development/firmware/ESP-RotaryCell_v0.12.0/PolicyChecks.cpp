#include "CallPolicy.h"
// Compile-time checks exercise the same policy helpers used by the firmware.
using namespace CallPolicy;
static_assert(unidentifiedAllowed(0,0) && unidentifiedAllowed(2,0));
static_assert(!unidentifiedAllowed(1,0));
static_assert(unidentifiedAllowed(1,1) && !unidentifiedAllowed(0,2));
static_assert(listedAllowed(0,false) && listedAllowed(1,true) && !listedAllowed(1,false));
static_assert(listedAllowed(2,false) && !listedAllowed(2,true));
static_assert(dailyWindow(1320,1320,360) && dailyWindow(359,1320,360));
static_assert(!dailyWindow(360,1320,360) && !dailyWindow(1319,1320,360));
static_assert(dailyWindow(720,600,900) && !dailyWindow(900,600,900));
static_assert(!dailyWindow(720,600,600));
static_assert(cliValidity("+CLIP: \"2125550123\",129")==-1);
static_assert(cliValidity("+CLIP: \"\",129,,,\"\",1")==1);
static_assert(cliValidity("+CLIP: \"\",129,,,\"\",2")==2);
static_assert(cliValidity("+CLIP: \"2125550123\",129,,,\"Last, First\",0")==0);
static_assert(cliValidity("+CLIP: \"\",129,,,\"\",9")==-1);
