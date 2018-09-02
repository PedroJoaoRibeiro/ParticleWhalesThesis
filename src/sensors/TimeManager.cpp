#include "TimeManager.h"


// initializes the rtc in the TIME_ZONE deffined in the config
TimeManager::TimeManager(){
  Config cfg = getDefaultConfig();
  rtc.begin(&UDPClient, "north-america.pool.ntp.org");
  rtc.setTimeZone(cfg.TIME_ZONE);
  lastTime = rtc.hour(rtc.now()) - 1;
}

// updates the currentTime value
void TimeManager::record(){
  currentTime = rtc.now();
}

// returns the value saved in currentTime in ISO format
String TimeManager::getRecordValue(){
  return rtc.ISODateUTCString(currentTime);
}

// returns the actual current time in ISO format
String TimeManager::getDateNow(){
  return rtc.ISODateUTCString(rtc.now());
}

// checks if it's possible to call the sensors for reading
// change this function if you want the sensors to read at a different time interval
// currently the sensors will record every hour
bool TimeManager::canReadSensors(){
  if(lastTime != rtc.hour(rtc.now())){
    Serial.println("Time is different");
    lastTime = rtc.hour(rtc.now());
    return true;
  }
  return false;
}
