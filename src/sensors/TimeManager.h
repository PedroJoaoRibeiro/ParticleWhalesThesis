#include "SensorsInterface.h"
#include "SparkTime/SparkTime.h"
#include "../config.h"



class TimeManager: public SensorsInterface {

  UDP UDPClient;
  SparkTime rtc;

  unsigned long currentTime;
  unsigned long lastTime = 0UL;
  String timeStr;

  public:
    // record the value from the sensor
    TimeManager();
    virtual void record();
    virtual String getRecordValue();
};
