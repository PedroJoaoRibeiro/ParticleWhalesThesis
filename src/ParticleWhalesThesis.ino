//------- Librarys Imports
#include "Particle.h"
#include "SdFat.h"
#include "softap_http.h"
#include "math.h"

//------- My Imports
#include "config.h"

#include "sensors/Bar100Sensor.h"
#include "sensors/TurbiditySensor.h"
#include "sensors/PHMeterSensor.h"
#include "sensors/GPSSensor.h"
#include "sensors/DissolvedOxygenSensor.h"
#include "sensors/TimeManager.h"

#include "sound/SoundManager.h"


#define BUFFER_SIZE 5120

Config cfg = getDefaultConfig();

//------- My Sensors Objects
Bar100Sensor* bar100Sensor;
TurbiditySensor* turbiditySensor;
PHMeterSensor* phMeterSensor;
GPSSensor* gpsSensor;
DissolvedOxygenSensor* dissolvedOxygenSensor;
TimeManager* timeManager;


//ENABLES threads needed for softap Mode
SYSTEM_THREAD(ENABLED)

//------- SDCARD SETTINGS
SdFatSoftSpi<D3, D2, D4> sd;
const uint8_t chipSelect = D5;
File myFile;

bool canRecord = false;


// recordingSoundState
enum State { STATE_STARTRECORDING, STATE_CONNECT, STATE_RUNNING, STATE_FINISH};
State state = STATE_STARTRECORDING;
unsigned long recordingStart;

// Forward Declaration of functions
void myPages(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved);


// setup() runs once, when the device is first turned on.
void setup() {
  setupSerial(); //allows output in serial Monitor
  waitUntil(Particle.connected);
  Serial.println("Connected to Particle ");


  connectSDCard();

  // register the cloud function
  Particle.function("enableap", enableSoftAp);
  Particle.function("enableread", enableReading);

  //Initialize Objects
  bar100Sensor = new Bar100Sensor(997); // kg/m^3 (freshwater, 1029 for seawater)
  turbiditySensor = new TurbiditySensor();
  phMeterSensor = new PHMeterSensor();
  gpsSensor = new GPSSensor();
  dissolvedOxygenSensor = new DissolvedOxygenSensor();
  timeManager = new TimeManager();


}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  if(canRecord){
    recordingSoundLoop();
  }



  //System.sleep(SLEEP_MODE_DEEP,1 * 60 * 1000); // 1 min

  //delay(60 * 1000); // 1 minute

}


// Reads the sensors values
void readSensors(){
  timeManager->record();
  Serial.println("Date:");
  Serial.println(timeManager->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  Serial.println("audioFile");
  Serial.println(getAudiFileName());

  Serial.println("----------------");
  Serial.println("----------------");

  gpsSensor->record();
  Serial.println("GPS    Lat, Long");
  Serial.println(gpsSensor->getRecordValue());


  Serial.println("----------------");
  Serial.println("----------------");


  bar100Sensor->record();
  Serial.println("pressure, temperature, depth, altitude");
  Serial.println(bar100Sensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  turbiditySensor->record();
  Serial.println("Turbidity  1 == HIGH || 0 == LOW");
  Serial.println(turbiditySensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");

  phMeterSensor->record();
  Serial.println("PH: ");
  Serial.println(phMeterSensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");



  dissolvedOxygenSensor->setTemperature(bar100Sensor->getTemperature());
  dissolvedOxygenSensor->record();
  Serial.println("DissolvedOxygenSensor: ");
  Serial.println(dissolvedOxygenSensor->getRecordValue());

  Serial.println("----------------");
  Serial.println("----------------");
}


void recordingSoundLoop(){
  switch (state) {
    // In this state we can start record
    case STATE_STARTRECORDING:
      connectSDCard();
      readSensors();
      recordingStart = millis();
      changeDirectory("Audio");
      startRecordingState(getAudiFileName());
      changeDirectory("/");
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
      saveRecordedData();
      state = STATE_STARTRECORDING;

      delay(60 * 1000);
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
  dataToWrite += getAudiFileName() + ",";                   //audioFile
  dataToWrite += gpsSensor->getRecordValue() + ",";         //gps -> latitude,longitude
  dataToWrite += bar100Sensor->getRecordValue() + ",";      //temperature,depth,altitude,pressure
  dataToWrite += turbiditySensor->getRecordValue() + ",";   //turbidity
  dataToWrite += phMeterSensor->getRecordValue() + ",";     //ph
  dataToWrite += dissolvedOxygenSensor->getRecordValue();   //oxygen

  Serial.println(System.freeMemory());
  Serial.println(dataToWrite);
  myFile.println(dataToWrite);
  myFile.close();

}

String getAudiFileName(){
  String aux = timeManager->getRecordValue().replace(":", "");
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

void changeDirectory(String path){
  if(!sd.chdir(path, true)){
    Serial.println("error changing directory: "+ path);
    sd.errorHalt("error changing directory");
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
    else if (strcmp(auxURl.substring(0,auxURl.indexOf("?")),"/audio")==0) {
        cb(cbArg, 0, 200, "text/json", nullptr);
        result->write(audioToSend(dataPart.substring(0,dataPart.indexOf("?")), dataPart.substring(dataPart.indexOf("?") + 1)));
    }
    else {
        cb(cbArg, 0, 404, nullptr, nullptr);
    }
}

String audioToSend(String fileName, String dataPart){
  connectSDCard();
  changeDirectory("/Audio");
  if (!myFile.open(fileName, O_READ)) {
    Serial.println("File Not found " + fileName);
    return "File Not found";
  }
  changeDirectory("/");

  int i;
  if (dataPart.length() == 0) {
    Serial.println("sending new data");
    i = 0;
  }
  else {
    i = dataPart.toInt();
    Serial.println("sending data from: " + dataPart);
  }

  // read from the file until there's nothing else in it:
  int data;

  String response = "";
  myFile.seek(i);

  while ((data = myFile.read()) >= 0) {
    Serial.println(data);
    char c = data;
    response += c;
    i ++;

    if (response.length() >= BUFFER_SIZE) {
      myFile.close();
      return getInitialJsonBodyAudio(i) + response + getFinalJsonbody(false);
    }
  }

  myFile.close();
  return getInitialJsonBodyAudio(i) + response + getFinalJsonbody(true);

}


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

    // open the file to read
    if (!myFile.open(cfg.fileName, O_READ)) {
      sd.errorHalt(("opening  " + cfg.fileName + " for read failed"));
    }

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
  jsonBody += cfg.DEVICE_ID;
  jsonBody += "\",\"dataPart\":";
  jsonBody += dataPart;
  jsonBody += ",\"csvData\":\"";
  return jsonBody;
}

String getInitialJsonBodyAudio(int dataPart){
  String jsonBody = "{\"dataPart\":";
  jsonBody += dataPart;
  jsonBody += ",\"audioData\":\"";
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
int enableSoftAp(String s){
  //starts listening Mode with a timout
  WiFi.listen();
  //WiFi.setListenTimeout(60);
  softap_set_application_page_handler(myPage, nullptr);
  Serial.println("Now Listening: ");
  return 0;
}

// Debug functions
int enableReading(String s){
  canRecord = true;
  return 0;
}
