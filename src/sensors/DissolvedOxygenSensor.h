#include "SensorsInterface.h"

class DissolvedOxygenSensor: public SensorsInterface {


  public:
    DissolvedOxygenSensor();
    virtual void record();
    virtual String getRecordValue();
};
