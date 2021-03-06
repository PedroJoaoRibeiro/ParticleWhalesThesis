//------- Librarys Imports
#include "Particle.h"
#include "SdFat.h"
#include "softap_http.h"
#include "math.h"
#include "Base64RK.h"


//------- My Imports
#include "config.h"

#include "sensors/Bar100Sensor.h"
#include "sensors/TurbiditySensor.h"
#include "sensors/PHMeterSensor.h"
#include "sensors/GPSSensor.h"
#include "sensors/DissolvedOxygenSensor.h"
#include "sensors/TimeManager.h"

#include "sound/SoundManager.h"

int BUFFER_SIZE = 5120;

Config cfg = getDefaultConfig();

//------- Needed sensors ----- Don't remove them
GPSSensor* gpsSensor;
TimeManager* timeManager;


//------- My Sensors Objects // Add New Sensors here
Bar100Sensor* bar100Sensor;
TurbiditySensor* turbiditySensor;
PHMeterSensor* phMeterSensor;
DissolvedOxygenSensor* dissolvedOxygenSensor;

int lock = 0;


//ENABLES threads needed for softap Mode
SYSTEM_THREAD(ENABLED)

//------- SDCARD SETTINGS
SdFatSoftSpi<D3, D2, D4> sd;
const uint8_t chipSelect = D5;
File myFile;

bool canRecord = false;
bool softAPMode = false;

// recordingSoundState
enum State { STATE_STARTRECORDING, STATE_RUNNING, STATE_FINISH};
State state = STATE_STARTRECORDING;
unsigned long recordingStart;

// Forward Declaration of functions
void myPages(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved);


// setup() runs once, when the device is first turned on.
void setup() {
  setupSerial(); //allows output in serial Monitor
  waitUntil(Particle.connected); // waitUntil network is available
  Serial.println("Connected to Particle ");



  // register the cloud function
  Particle.function("enableap", enableSoftAp); // function to start AP Mode
  Particle.function("enableread", enableReading); // function to start recording Mode

  // Needed Objects
  gpsSensor = new GPSSensor();
  timeManager = new TimeManager();

  //Initialize your Objects
  bar100Sensor = new Bar100Sensor(997); // kg/m^3 (freshwater, 1029 for seawater)
  turbiditySensor = new TurbiditySensor();
  phMeterSensor = new PHMeterSensor();
  dissolvedOxygenSensor = new DissolvedOxygenSensor();

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  if(canRecord){
    recordingSoundLoop();
  }
  // checks if the current depth is less than 1 meter, if it is
  // disables the reading mode and turns the softAP mode on
  if(softAPMode == true){

  }

  // checks if current depth is bellow 1 meter and if softAP is on, if it is
  // disables the softAP mode and turns the reading mode on
  /*if(bar100Sensor->getDepth() < -1 && softAPMode == true){
    canRecord = true;

    softAPMode = false;

    WiFi.listen(false); // Disables Listening Mode
  }*/
}


// Reads the sensors values
void readSensors(){
  timeManager->record();

  gpsSensor->record();

  bar100Sensor->record();

  turbiditySensor->record();

  phMeterSensor->record();

  dissolvedOxygenSensor->setTemperature(bar100Sensor->getTemperature());
  dissolvedOxygenSensor->record();

  // dissolvedOxygenSensor needs 10 values before reading is correct
  if(dissolvedOxygenSensor->getRecordValue() == "0.00,"){
    delay(100);
    readSensors();
  }

}

// print result of sensors for debugging
void printResults(){
  Serial.println("Date:");
  Serial.println(timeManager->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  Serial.println("GPS    Lat, Long");
  Serial.println(gpsSensor->getRecordValue());


  Serial.println("----------------");
  Serial.println("----------------");

  Serial.println("temperature, depth, altitude, pressure");
  Serial.println(bar100Sensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  Serial.println("Turbidity  1 == HIGH || 0 == LOW");
  Serial.println(turbiditySensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  Serial.println("PH: ");
  Serial.println(phMeterSensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  Serial.println("DissolvedOxygenSensor: ");
  Serial.println(dissolvedOxygenSensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  String dataToWrite = timeManager->getRecordValue() + ","; //date
  dataToWrite += gpsSensor->getRecordValue() + ",";         //gps -> latitude,longitude
  dataToWrite += bar100Sensor->getRecordValue() + ",";      //temperature,depth,altitude,pressure
  dataToWrite += turbiditySensor->getRecordValue() + ",";   //turbidity
  dataToWrite += phMeterSensor->getRecordValue() + ",";     //ph
  dataToWrite += dissolvedOxygenSensor->getRecordValue();   //oxygen

  Serial.println(dataToWrite);
}

// recordingSoundLoop in the end of the loop calls the timeManager to check if we can read the sensors
void recordingSoundLoop(){
  switch (state) {
    // In this state we can start record
    case STATE_STARTRECORDING:
      connectSDCard();
      changeDirectory("Audio");
      startRecordingState(getAudiFileName());
      recordingStart = millis();
      state = STATE_RUNNING;
      break;

    // In this state we are waiting for the microphone record to finish
    case STATE_RUNNING:
      // check to see if if the recording time exceeded the maximum time
      if (millis() - recordingStart >= cfg.MAX_RECORDING_LENGTH_MS) {
        state = STATE_FINISH;
      } else {
        recordingState();
      }
      break;

    // In this state we save all the sensors information to the SdCard
    // Then setDelay for the next reading to start
    case STATE_FINISH:
      finishState();
      changeDirectory("/");
      state = STATE_STARTRECORDING;

      if(timeManager->canReadSensors()){
        readSensors();
        saveRecordedData();
      }
      delay(1000);

      break;
  }
}

// Current Data
//"date,audioFile,latitude,longitude,temperature,depth,altitude,pressure,turbidity,ph,oxygen"
void saveRecordedData(){
  if (!sd.exists(cfg.fileName)) {
    Serial.println("File don't exists creating it now: " + cfg.fileName);
    // open the file for write at end like the "Native SD library"
    if (!myFile.open(cfg.fileName, O_RDWR | O_CREAT | O_AT_END)) {
      sd.errorHalt(("opening " + cfg.fileName + " for write failed"));
    }
    myFile.println(cfg.csvInitialString);
  }
  else {
    if (!myFile.open(cfg.fileName, O_RDWR | O_CREAT | O_AT_END)) {
      sd.errorHalt(("opening " + cfg.fileName + " for write failed"));
    }
  }
  String dataToWrite = timeManager->getRecordValue() + ","; //date
  dataToWrite += gpsSensor->getRecordValue() + ",";         //gps -> latitude,longitude
  dataToWrite += bar100Sensor->getRecordValue() + ",";      //temperature,depth,altitude,pressure
  dataToWrite += turbiditySensor->getRecordValue() + ",";   //turbidity
  dataToWrite += phMeterSensor->getRecordValue() + ",";     //ph
  dataToWrite += dissolvedOxygenSensor->getRecordValue();   //oxygen

  Serial.println(dataToWrite);
  myFile.println(dataToWrite);
  myFile.close();

}

// gets a name for the audioFile based on the Currentdate
String getAudiFileName(){
  String aux = timeManager->getDateNow().replace(":", "");
  return cfg.DEVICE_ID + aux + ".wav";
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

// changes directory inside the SD card
void changeDirectory(String path){
  if(!sd.chdir(path, true)){
    Serial.println("error changing directory: "+ path);
    sd.errorHalt("error changing directory");
  }
}

// function than handles the requests to the device while in softap Mode
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

// generates a string with the response to the client,
// based on the last part of the data that the client received
String dataToSend(String dataPart){
    connectSDCard();
    int i;
    if (dataPart.length() == 0) {
      Serial.println("sending new data");
      i = 0;
    }
    else {
      i = dataPart.toInt();
      Serial.println("sending data from: " + dataPart);
    }

    String response = "";

    WITH_LOCK(Serial){
      // open the file to read
      if (!myFile.open(cfg.fileName, O_READ)) {
        sd.errorHalt(("opening  " + cfg.fileName + " for read failed"));
      }

      // read from the file until there's nothing else in it:
      int data;


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
      Serial.println("done");
    }
    return getInitialJsonBody(i) + response + getFinalJsonbody(true);;
}

// generates the initial jsonBody
String getInitialJsonBody(int dataPart){
  String jsonBody = "{ \"deviceId\": \"";
  jsonBody += cfg.DEVICE_ID;
  jsonBody += "\",\"dataPart\":";
  jsonBody += dataPart;
  jsonBody += ",\"csvData\":\"";
  return jsonBody;
}

// generates the final jsonBody
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
int enableSoftAp(String s){
  //starts listening Mode with a timout
  if (canRecord) {
    canRecord = false;
  }
  WiFi.listen();
  softAPMode = true;
  softap_set_application_page_handler(myPage, nullptr);
  Serial.println("Now Listening: ");
  return 0;
}



// Debug functions
int enableReading(String s){
  canRecord = !canRecord;
  if (canRecord) {
    return 0;
  }
  return 1;
}
