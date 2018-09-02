#include "SdFat.h"
#include "Particle.h"
#include "../config.h"
#include "application.h"

#include "SparkIntervalTimer.h"


#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H


//----- FUNCTIONS
void writeWavHeader(String fileName); // writes the header at the beginning of the .wav file
void writeOutHeader();  // writes the out header at the end of the .wav file
void timerISR(); // timer function to read the data from the microphone

void startRecordingState(String fileName);  // function that manages the state STATE_STARTRECORDING
void recordingState();  // function that manages the state STATE_RUNNING
void finishState();   // function that manages the state STATE_FINISH


#endif
