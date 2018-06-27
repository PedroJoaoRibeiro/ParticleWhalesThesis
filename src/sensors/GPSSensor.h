#include "SensorsInterface.h"
#include "TinyGPS/TinyGPS.h"

class GPSSensor: public SensorsInterface {

  TinyGPS gps;
  char szInfo[64];

  public:
    GPSSensor();
    virtual void record();
    virtual String getRecordValue();
};
