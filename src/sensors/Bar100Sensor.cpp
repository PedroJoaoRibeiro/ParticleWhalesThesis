#include "Bar100Sensor.h"


// Public Methods Defenition
Bar100Sensor::Bar100Sensor(int fluidDensity){
  _fluidDensity = fluidDensity;

  Wire.begin();

  sensor.init();
  sensor.setFluidDensity(fluidDensity);

  if (sensor.isInitialized()) {
    Serial.println("Sensor connected.");
  } else {
    Serial.println("Sensor not connected.");
  }
}

void Bar100Sensor::record(){
  sensor.read();
}

String Bar100Sensor::getRecordValue(){
  String str = String(sensor.pressure()) + ",";
  str += String(sensor.temperature()) + ",";
  str += String(sensor.depth()) + ",";
  str += String(sensor.altitude()) + ",";
  return str;
}
