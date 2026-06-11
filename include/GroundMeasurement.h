#ifndef GROUND_MEASUREMENT_H
#define GROUND_MEASUREMENT_H

#include <Arduino.h>

void soilInit();
int  getSoilMoisture();
int  getSoilMoistureDelta();

#endif