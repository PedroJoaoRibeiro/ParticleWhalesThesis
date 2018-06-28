#include "TimeManager.h"


// Public Methods Defenition
TimeManager::TimeManager(){
  Config cfg = getDefaultConfig();
  Serial.println("here");
  rtc.begin(&UDPClient, "north-america.pool.ntp.org");
  rtc.setTimeZone(cfg.TIME_ZONE); // gmt offset
}

void TimeManager::record(){
  currentTime = rtc.now();
}

String TimeManager::getRecordValue(){
  return rtc.ISODateUTCString(currentTime);
}
