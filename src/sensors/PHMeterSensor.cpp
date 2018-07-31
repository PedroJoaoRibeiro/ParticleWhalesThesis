#include "PHMeterSensor.h"



// Public Methods Defenition
PHMeterSensor::PHMeterSensor(){
}

void PHMeterSensor::record(){
  unsigned long int avgValue;  //Store the average value of the sensor feedback
  float b;
  int buf[10],temp;

  for(int i=0;i<10;i++){       //Get 10 sample value from the sensor for smooth the value
    buf[i]=analogRead(phSensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++){        //sort the analog from small to large
    for(int j=i+1;j<10;j++){
      if(buf[i]>buf[j]){
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  ph = phValue + offset;
}

String PHMeterSensor::getRecordValue(){
  return String(ph,2);
}
