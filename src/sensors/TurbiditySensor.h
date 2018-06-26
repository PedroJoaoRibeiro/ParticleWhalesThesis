#include "SensorsInterface.h"


class TurbiditySensor: public SensorsInterface {

  //------- PIN
  int turbiditySensorPin = D5;
  int turbidity;

  public:
    TurbiditySensor();
    virtual void record();
    virtual String getRecordValue();
};
