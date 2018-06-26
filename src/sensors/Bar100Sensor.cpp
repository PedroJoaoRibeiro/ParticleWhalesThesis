#include "Bar100Sensor.h"


// Public Methods Defenition
Bar100Sensor::Bar100Sensor(int fluidDensity){
  fluidDensity = fluidDensity
}

void Bar100Sensor::record(){
  Serial.println("recording");
}
