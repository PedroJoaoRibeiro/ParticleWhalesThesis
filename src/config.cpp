#include "Particle.h"
#include "config.h"

Config getDefaultConfig(){

	Config c = {
		"PhotonTest1",	//Id of Device should be unique for each photon
		12,							//Pin for the sd card
		"data.csv",		//FileName
		"date,latitude,longitude,temperature,depth,altitude,pressure,turbidity,ph,oxygen", 						//Initial String of csvData change this to conform sensors available
		0,							// Timezone 0 for gmt then + or - for each zone

		5120, 					//BUFFER_SIZE


		// recording vars
		4000, 					// Maximum Recording Time
		16000,					// Audio Sample Rate
		11,							// PIN A1

		8000,						// sampleRate;
		16,							// sampleBufferSizeKB;
		16000,					// bytesPerSec = sampleRate * 2
		131072,					// dataSize = config.DataSize(128 KB) * 1024; This value MUST ALWAYS be a power of 2
		131116,					// fileSize = config.dataSize + 44 (Wave header)
	};
	return c;
}
