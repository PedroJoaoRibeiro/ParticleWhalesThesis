#include "Particle.h"

#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

// The code for each sensor object must be implemented by complying with this interface
// So every sensor object must implement this interface and their respective Methods.
class SensorsInterface  {

  public:
    // record the value from the sensor for later use
    virtual void record() = 0;

    // gets the value from the sensor as a string.
    // If you want to retur more than one value for a specific sensor they should be
    // separed by a comma
    virtual String getRecordValue() = 0;
};

#endif
