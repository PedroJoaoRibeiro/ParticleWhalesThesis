#include "SensorsInterface.h"

// Turbidity Sensor
class TurbiditySensor: public SensorsInterface {

  //------- PIN
  int turbiditySensorPin = D5;  // Pin where the sensor is connected
  int turbidity;  // turbidity value

  public:
    TurbiditySensor();  // object constructor
    virtual void record();  // record the current turbidity value
    virtual String getRecordValue();  // returns the turbidity value recorded as a string
};
