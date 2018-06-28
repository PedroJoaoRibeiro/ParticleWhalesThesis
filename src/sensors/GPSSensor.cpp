#include "GPSSensor.h"


// Public Methods Defenition
GPSSensor::GPSSensor(){
  Serial1.begin(9600);
}

void GPSSensor::record(){
  bool isValidGPS = false;

    for (unsigned long start = millis(); millis() - start < 1000;){
        // Check GPS data is available
        while (Serial1.available()){
            char c = Serial1.read();
            // parse GPS data
            if (gps.encode(c))
                isValidGPS = true;
        }
    }

    // If we have a valid GPS location then publish it
    if (isValidGPS){
        float lat, lon;
        unsigned long age;

        gps.f_get_position(&lat, &lon, &age);

        sprintf(szInfo, "%.6f,%.6f", (lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat), (lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon));
    }
    else{
        // Not a vlid GPS location, jsut pass 0.0,0.0
        // This is not correct because 0.0,0.0 is a valid GPS location, we have to pass a invalid GPS location
        // and check it at the client side
        sprintf(szInfo, "0.0,0.0");
    }

}

String GPSSensor::getRecordValue(){
  String str = szInfo;
  return str + ",";
}
