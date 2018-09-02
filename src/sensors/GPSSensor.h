#include "SensorsInterface.h"
#include "TinyGPS/TinyGPS.h"


// Gps sensor using the TinyGPS Library
class GPSSensor: public SensorsInterface {

  TinyGPS gps;
  char szInfo[64];

  public:
    GPSSensor();  // initializes the sensor
    virtual void record();  // records the value and saves it
    virtual String getRecordValue();  // returns the value recorded as a String
};
