#include "TurbiditySensor.h"



// Public Methods Defenition

// Initializes the sensor in the respective pin
TurbiditySensor::TurbiditySensor(){
  pinMode(turbiditySensorPin, INPUT);
}

// reads the value from the pin and saves it
void TurbiditySensor::record(){
  // if 1 == HIGH if 0 == LOW
  turbidity = digitalRead(turbiditySensorPin);
}

// returns the turbidity value that is saved
String TurbiditySensor::getRecordValue(){
  return String(turbidity);
}
