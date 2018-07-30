#include "SdFat.h"
#include "Particle.h"
#include "../config.h"
#include "application.h"

#include "SparkIntervalTimer.h"


#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H


//----- FUNCTIONS
void recordingSoundLoop(String fileName);
void StartRec();
void StopRec();
void writeWavHeader(String fileName);
void writeOutHeader();
void timerISR(); // timer function to read the data from the microphone

void startRecordingState(String fileName);
void recordingState();
void finishState();


#endif
