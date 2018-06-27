//------- Librarys Imports
#include "Particle.h"
#include "SdFat.h"
#include "softap_http.h"
#include "math.h"

//------- My Imports
#include "sensors/Bar100Sensor.h"
#include "sensors/TurbiditySensor.h"
#include "sensors/PHMeterSensor.h"
#include "sensors/GPSSensor.h"
#include "sensors/DissolvedOxygenSensor.h"

//------- My Sensors Objects
TurbiditySensor turbiditySensor;
PHMeterSensor phMeterSensor;
GPSSensor gpsSensor;
DissolvedOxygenSensor dissolvedOxygenSensor;


//ENABLES threads needed for softap Mode
SYSTEM_THREAD(ENABLED)

//------- SDCARD SETTINGS

SdFatSoftSpi<D3, D2, D4> sd;
const uint8_t chipSelect = D5;
File myFile;
String fileName = "data.csv";

#define BUFFER_SIZE 5120

// Forward Declaration of functions
void myPages(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved);






// setup() runs once, when the device is first turned on.
void setup() {
  setupSerial(); //allows output in serial Monitor
  waitUntil(Particle.connected);
  Serial.println("Connected to Particle ");


  connectSDCard();


  // register the cloud function
  Particle.function("debug", debug);

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {


  readSensors();


  //System.sleep(SLEEP_MODE_DEEP,1 * 60 * 1000); // 1 min

  delay(60 * 1000); // 10 minutes

}


// Reads the sensors values
void readSensors(){
  Bar100Sensor bar(997); // kg/m^3 (freshwater, 1029 for seawater)
  bar.record();
  Serial.println("pressure, temperature, depth, altitude");
  Serial.println(bar.getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  turbiditySensor.record();
  Serial.println("Turbidity  1 == HIGH || 0 == LOW");
  Serial.println(turbiditySensor.getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  phMeterSensor.record();
  Serial.println("PH: ");
  Serial.println(phMeterSensor.getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  gpsSensor.record();
  Serial.println("GPS    Lat, Long");
  Serial.println(gpsSensor.getRecordValue());


  Serial.println("----------------");
  Serial.println("----------------");

  dissolvedOxygenSensor.record();
  Serial.println("DissolvedOxygenSensor: ");
  Serial.println(dissolvedOxygenSensor.getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");
}


// Sets the serial to enable debugging
void setupSerial(){
  //--------------------- Serial setup
  Serial.begin(9600);
  // Wait for USB Serial
  while (!Serial) {
    SysCall::yield();
  }
}

// connects the sdCard
void connectSDCard(){
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // Change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }
}

void myPage(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved)
{
    Serial.printlnf("handling page %s", url);

    if (strcmp(url,"/index")==0) {
        Serial.println("sending redirect");
        Header h("Location: /index.html\r\n");
        cb(cbArg, 0, 301, "text/plain", &h);
        return;
    }

    String auxURl = url;
    String dataPart = auxURl.substring(auxURl.indexOf("?") + 1, auxURl.length());
    Serial.println("dataPat: " + dataPart);

    if (strcmp(auxURl.substring(0,auxURl.indexOf("?")),"/data")==0) {
        cb(cbArg, 0, 200, "text/json", nullptr);
        result->write(dataToSend(dataPart));
    }
    else {
        cb(cbArg, 0, 404, nullptr, nullptr);
    }
}

String dataToSend(String dataPart){
    int i;

    if (dataPart.length() == 0) {
      Serial.println("sending new data");
      i = 0;
    }
    else {
      i = dataPart.toInt();
      Serial.println("sending data from: " + dataPart);
    }

    // open the file to read
    if (!myFile.open(fileName, O_READ)) {
      sd.errorHalt(("opening  " + fileName + " for read failed"));
    }
    Serial.println((fileName + " content:"));

    // read from the file until there's nothing else in it:
    int data;

    String response = "";
    myFile.seek(i);

    while ((data = myFile.read()) >= 0) {
      char c = data;
      if (c == '\n' || c == '\r') { // substitutes \n and \r to ; so that json can be accepted
        if (!response.endsWith(";")) {
          response += ";";
        }
      } else {
        response += c;
      }
      i ++;

      if (response.length() >= BUFFER_SIZE) {
        myFile.close();
        return getInitialJsonBody(i) + response + getFinalJsonbody(false);
      }
    }

    myFile.close();
    return getInitialJsonBody(i) + response + getFinalJsonbody(true);
}

String getInitialJsonBody(int dataPart){
  String jsonBody = "{ \"deviceId\": \"";
  jsonBody += "PhotonTest1";
  jsonBody += "\",\"dataPart\":";
  jsonBody += dataPart;
  jsonBody += ",\"csvData\":\"";
  return jsonBody;
}

String getFinalJsonbody(bool b){
  String jsonBody = "\",\"isFinalData\":";
  if (b) {
    jsonBody += "true";
  } else {
    jsonBody += "false";
  }

  jsonBody += "}";
  return jsonBody;
}

// Debug functions
int debug(String s){
  //starts listening Mode with a timout
  WiFi.listen();
  //WiFi.setListenTimeout(60);
  //softap_set_application_page_handler(myPage, nullptr);
  Serial.println("Now Listening: ");
  return 0;
}
