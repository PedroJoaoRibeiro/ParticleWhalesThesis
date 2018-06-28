#include "TurbiditySensor.h"



// Public Methods Defenition
TurbiditySensor::TurbiditySensor(){
  pinMode(turbiditySensorPin, INPUT);
}

void TurbiditySensor::record(){
  // if 1 == HIGH if 0 == LOW
  turbidity = digitalRead(turbiditySensorPin);
}

String TurbiditySensor::getRecordValue(){
  return String(turbidity) + ",";
}
