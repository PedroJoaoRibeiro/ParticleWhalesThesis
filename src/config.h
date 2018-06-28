#ifndef MYSTRUCT_H
#define MYSTRUCT_H

struct Config {
  // Device configs
  String DEVICE_ID; // Id of Device should be unique for each photon
  int SDCARD_PIN;   // Pin for the sd card
  String fileName; // Name of the file to save the data
  String csvInitialString; // Initial String of csvData change this to conform sensors available
  int TIME_ZONE;   // Timezone 0 for gmt then + or - for each zone

	int BUFFER_SIZE; // Maximum Size of the buffer to send data


	// recording vars
	unsigned long MAX_RECORDING_LENGTH_MS;  // Maximum Recording Time
  long SAMPLE_RATE;                       // Audio Sample Rate -> Minimum 8k maximum 48k because of timer overhead
  int MICRO_PIN;                          // Microphone PIN

  ushort sampleRate;
  ushort sampleBufferSize;
  ushort bytesPerSec;
  size_t dataSize;
  size_t fileSize;
};

Config getDefaultConfig();

#endif
