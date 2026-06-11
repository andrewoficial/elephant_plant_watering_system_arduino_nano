#ifndef AIR_MEASUREMENT_H
#define AIR_MEASUREMENT_H

#include <Arduino.h>

void airInit();
float getAirTemperature();
float getAirHumidity();
float getAirPressure(); // Оставляем для совместимости команд, всегда вернет 0

#endif