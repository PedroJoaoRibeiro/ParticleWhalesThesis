#include "SensorsInterface.h"
#include "SparkTime/SparkTime.h"
#include "../config.h"


// Class that manages the time
// Uses the SparkTime library and needs internet connection from the device in the
// beginning in order to get the current Time
class TimeManager: public SensorsInterface {

  UDP UDPClient;  // UDP client to connect to the server
  SparkTime rtc;  // RTC object

  unsigned long currentTime;
  unsigned long lastTime;
  String timeStr;

  public:
    TimeManager();  // initializes the rtc
    virtual void record(); // record the value from the sensor
    virtual String getRecordValue(); // returns the date in string format
    String getDateNow();  // gets the current date
    bool canReadSensors();  // checks if it's possible to read the current time

};
