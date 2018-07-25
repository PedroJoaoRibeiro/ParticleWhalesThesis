#include "SensorsInterface.h"
#include "SparkTime/SparkTime.h"
#include "../config.h"



class TimeManager: public SensorsInterface {

  UDP UDPClient;
  SparkTime rtc;

  unsigned long currentTime;
  unsigned long lastTime;
  String timeStr;

  public:
    // record the value from the sensor
    TimeManager();
    virtual void record();
    virtual String getRecordValue();
    String getDateNow();
    bool canReadSensors();

};
