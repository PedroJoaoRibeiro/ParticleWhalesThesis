//------- Librarys Imports
#include "Particle.h"
#include "SdFat.h"
#include "softap_http.h"



//ENABLES threads needed for softap Mode
SYSTEM_THREAD(ENABLED)

//------- SDCARD SETTINGS
SdFat sd(1);
const uint8_t chipSelect = D1;
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
  delay(5000);
  Serial.println("here");
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
  softap_set_application_page_handler(myPage, nullptr);
  Serial.println("Now Listening: ");
  return 0;
}