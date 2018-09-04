#include "Particle.h"
#include "SensorsInterface.h"
#include "KellerLD.h"

// Implementation of the Bar100Sensor
class Bar100Sensor: public SensorsInterface {

  // Fluid density should be changed if we are dealing with salt water
  int _fluidDensity;

  // object of the creators' library
  KellerLD sensor;

  public:
    // constructor class where the fluidDensity is set
    Bar100Sensor(int fluidDensity);

    // makes the sensor record the values
    virtual void record();

    // returns all the values as a string
    virtual String getRecordValue();

    // returns the current temperature value that's needed by other sensors
    float getTemperature();

    // returns the current depth value that's needed by the main loop
    float getDepth();
};
