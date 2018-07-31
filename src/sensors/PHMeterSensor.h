#include "SensorsInterface.h"

class PHMeterSensor: public SensorsInterface {
  int phSensorPin = A2;
  float offset = -33.3;
  float ph;

  public:
    PHMeterSensor();
    virtual void record();
    virtual String getRecordValue();

};
