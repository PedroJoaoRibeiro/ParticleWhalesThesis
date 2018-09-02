#include "Bar100Sensor.h"


// Public Methods Defenition

// constructor method that initializes the sensor and set its fluidDensity
Bar100Sensor::Bar100Sensor(int fluidDensity){
  _fluidDensity = fluidDensity;

  Wire.begin();

  sensor.init();
  sensor.setFluidDensity(fluidDensity);

  if (sensor.isInitialized()) {
    Serial.println("Bar100 Sensor connected.");
  } else {
    Serial.println("Sensor not connected.");
  }
}

// reads all the values from the sensor and saves them in the sensor object
void Bar100Sensor::record(){
  sensor.read();
}

// returns all the values in a string, where each value is separated by a comma
String Bar100Sensor::getRecordValue(){
  String str = String(sensor.temperature(),2) + ",";
  str += String(sensor.depth(),2) + ",";
  str += String(sensor.altitude(),2) + ",";
  str += String(sensor.pressure() * 0.001,2); // convert to bar
  return str;
}

// returns the current temperature
float Bar100Sensor::getTemperature(){
  return sensor.temperature();
}
