#include "TimeManager.h"


// Public Methods Defenition
TimeManager::TimeManager(){
  Config cfg = getDefaultConfig();
  rtc.begin(&UDPClient, "north-america.pool.ntp.org");
  rtc.setTimeZone(cfg.TIME_ZONE); // gmt offset
  lastTime = rtc.hour(rtc.now()) - 1;
}

void TimeManager::record(){
  currentTime = rtc.now();
}

String TimeManager::getRecordValue(){
  return rtc.ISODateUTCString(currentTime);
}

String TimeManager::getDateNow(){
  return rtc.ISODateUTCString(rtc.now());
}

bool TimeManager::canReadSensors(){
  if(lastTime != rtc.hour(rtc.now())){
    Serial.println("Time is different");
    lastTime = rtc.hour(rtc.now());
    return true;
  }
  return false;
}
