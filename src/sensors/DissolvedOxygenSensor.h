#include "SensorsInterface.h"
#include "avr/pgmspace.h"

#define VREF 5000    //for arduino uno, the ADC reference is the AVCC, that is 5000mV(TYP)

#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

#define ReceivedBufferLength 20
#define SCOUNT  30           // sum of sample point

#define SaturationDoVoltageAddress 12          //the address of the Saturation Oxygen voltage stored in the EEPROM
#define SaturationDoTemperatureAddress 16      //the address of the Saturation Oxygen temperature stored in the EEPROM

class DissolvedOxygenSensor: public SensorsInterface {
  int DoSensorPin = A5;
  float doValue;      //current dissolved oxygen value, unit; mg/L
  float temperature;

  char receivedBuffer[ReceivedBufferLength+1];    // store the serial command
  byte receivedBufferIndex = 0;

  int analogBuffer[SCOUNT];    //store the analog value in the array, readed from ADC
  int analogBufferTemp[SCOUNT];
  int analogBufferIndex = 0,copyIndex = 0;

  float SaturationDoVoltage,SaturationDoTemperature;
  float averageVoltage;

  const float SaturationValueTab[41] PROGMEM = {      //saturation dissolved oxygen concentrations at various temperatures
    14.46, 14.22, 13.82, 13.44, 13.09,
    12.74, 12.42, 12.11, 11.81, 11.53,
    11.26, 11.01, 10.77, 10.53, 10.30,
    10.08, 9.86,  9.66,  9.46,  9.27,
    9.08,  8.90,  8.73,  8.57,  8.41,
    8.25,  8.11,  7.96,  7.82,  7.69,
    7.56,  7.43,  7.30,  7.18,  7.07,
    6.95,  6.84,  6.73,  6.63,  6.53,
    6.41,
    };

  public:
    DissolvedOxygenSensor();
    virtual void record();
    virtual String getRecordValue();

    void setTemperature(float temp);

  private:
    boolean serialDataAvailable(void);
    byte uartParse();
    void doCalibration(byte mode);
    int getMedianNum(int bArray[], int iFilterLen);
    void readDoCharacteristicValues(void);
};
