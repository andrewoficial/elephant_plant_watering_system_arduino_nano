#ifndef GROUND_WATERING_H
#define GROUND_WATERING_H

#include <Arduino.h>

void initWatering();
void startPump(unsigned int seconds);
void stopPump();
void updatePump();
bool isPumpRunning();
void startWaterDetection();
bool checkWaterFlow();
void stopWaterDetection();
void getWaterDetectionResult(char* buffer, size_t bufSize);

#endif