#include "Particle.h"

#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

class SensorsInterface  {

  public:
    // record the value from the sensor
    virtual void record() = 0;
    virtual String getRecordValue() = 0;
};

#endif
