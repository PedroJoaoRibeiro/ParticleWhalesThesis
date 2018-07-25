#include "SoundManager.h"
//------Files Creation-------------------//
//Config CONFIG = getDefaultConfig();  //Read the default file
File rec;
IntervalTimer timer;

Config CONFIG = getDefaultConfig();


//------------Variable for wav file-------------------//
#define ADC_CENTER 2060

#define WAVE_CHUNK 16
#define WAVE_TYPE 1
#define NUM_CHANNELS 1
#define BLOCK_ALIGN 2  //numchannel*bitpersample/8
#define BITS_PER_SAMPLE 16

byte byte1, byte2, byte3, byte4;

//------Structure to hold the data timer-------//
typedef struct {
	volatile bool free = true;
	volatile size_t  index = 0;
	byte data[1024]; // = new byte[24576];
} SampleBuf;

SampleBuf buffers[2]; // = new SampleBuf[CONFIG.numberBuffers];
volatile size_t sampleIndex = 0;
volatile size_t sendIndex = 0;

//----------buffer to send byte for server/TCP-----//
byte buf00[1024];

//----- AUX VARS
int buffercount = 0;
int writer = 0;
int sendFile = 1;
short sample = 0;


//----- Main Recording loop

void startRecordingState(String fileName){
	Serial.println("Start Recording audio");
	Serial.println(fileName);
	writeWavHeader(fileName);
	// 8000 samples/sec = 125 microseconds (The minimum timer period is about 10 micrseconds)
	timer.begin(timerISR, 1000000 / CONFIG.sampleRate, uSec);
}

void recordingState(){
	if (sendIndex < sampleIndex) {
	// There is a sample buffer to send
		SampleBuf *sb = &buffers[sendIndex % 2];
		writer = 1;
		rec.write(sb->data,CONFIG.sampleBufferSize);
		writer = 0;
		buffercount++;
		sb->free = true;
		sendIndex++;
	}

	/*if (buffercount >= CONFIG.bufferToWrite) {
		// size bigger than expected need to stop recording
		state = STATE_FINISH;

	}*/
}

void finishState(){
	Serial.println("Finish Recording");

	timer.end();
	sampleIndex = 0;
  writeOutHeader();

	buffercount = 0;
	sendIndex = 0;
	sendFile = 0;
}



void writeWavHeader(String fileName) {

	if (!rec.open(fileName, O_CREAT | O_TRUNC | O_RDWR)) {
		Serial.println("error opening file: " + fileName);
  }

  rec.write("RIFF");
  byte1 = (CONFIG.fileSize-8) & 0xff;
  byte2 = ((CONFIG.fileSize-8) >> 8) & 0xff;
  byte3 = ((CONFIG.fileSize-8) >> 16) & 0xff;
  byte4 = ((CONFIG.fileSize-8) >> 24) & 0xff;
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  rec.write("WAVE");
  rec.write("fmt ");
  byte1 = WAVE_CHUNK & 0xff;
  byte2 = (WAVE_CHUNK >> 8) & 0xff;
  byte3 = (WAVE_CHUNK >> 16) & 0xff;
  byte4 = (WAVE_CHUNK >> 24) & 0xff;
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  byte1 = WAVE_TYPE & 0xff;
  byte2 = (WAVE_TYPE >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2);
  byte1 = NUM_CHANNELS & 0xff;
  byte2 = (NUM_CHANNELS >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2);
  byte1 = CONFIG.sampleRate & 0xff;
  byte2 = (CONFIG.sampleRate >> 8) & 0xff;
  byte3 = (CONFIG.sampleRate >> 16) & 0xff;
  byte4 = (CONFIG.sampleRate >> 24) & 0xff;
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  byte1 = CONFIG.bytesPerSec & 0xff;
  byte2 = (CONFIG.bytesPerSec >> 8) & 0xff;
  byte3 = (CONFIG.bytesPerSec >> 16) & 0xff;
  byte4 = (CONFIG.bytesPerSec >> 24) & 0xff;
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  byte1 = BLOCK_ALIGN & 0xff;
  byte2 = (BLOCK_ALIGN >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2);
  byte1 = BITS_PER_SAMPLE & 0xff;
  byte2 = (BITS_PER_SAMPLE >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2);
  rec.write("data");
  byte1 = CONFIG.dataSize & 0xff;
  byte2 = (CONFIG.dataSize >> 8) & 0xff;
  byte3 = (CONFIG.dataSize >> 16) & 0xff;
  byte4 = (CONFIG.dataSize >> 24) & 0xff;
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
}

void writeOutHeader(){
	rec.close();
}

void timerISR() {
	// This is an interrupt service routine. Don't put any heavy calculations here
	// or call anything that's not interrupt-safe, such as:
	// Serial, String, any memory allocation (new, malloc, etc.), Particle.publish and other Particle methods, and more.
	if(writer == 0){
		SampleBuf *sb = &buffers[sampleIndex % 2];

		if (!sb->free) {
			// The network has fallen behind in sending so discard samples
			return;
		}

	  sample = analogRead(CONFIG.MICRO_PIN)-ADC_CENTER;
		// Convert 12-bit sample to 16-bit
	  sb->data[sb->index++] = (uint8_t)(sample & 0xFF);
		sb->data[sb->index++] = (uint8_t)((sample>>8) & 0xFF);

		if (sb->index >= CONFIG.sampleBufferSize) {
			// Buffer has been filled.
			sb->free = false;
			sb->index = 0;
			sampleIndex++;
	  }
	}
}
