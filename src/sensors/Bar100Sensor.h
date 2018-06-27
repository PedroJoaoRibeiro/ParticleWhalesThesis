#include "Particle.h"
#include "SensorsInterface.h"
#include "KellerLD.h"


class Bar100Sensor: public SensorsInterface {

  int _fluidDensity;
  KellerLD sensor;

  public:
    Bar100Sensor(int fluidDensity);
    virtual void record();
    virtual String getRecordValue();

    float getTemperature();
};
