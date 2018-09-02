#include "SensorsInterface.h"

// pH Meter sensor
class PHMeterSensor: public SensorsInterface {

  int phSensorPin = A2; // Pin where the sensor is connected
  float offset = 0.35;  // offset value chosen after calibration
  float ph; // current pH value

  public:
    PHMeterSensor();   // object constructor
    virtual void record();  // record the current pH value
    virtual String getRecordValue();  // returns the pH value recorded as a string
};
