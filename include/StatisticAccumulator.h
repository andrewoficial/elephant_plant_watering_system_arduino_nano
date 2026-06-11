#ifndef STATISTIC_ACCUMULATOR_H
#define STATISTIC_ACCUMULATOR_H

#include <Arduino.h>

struct StatData {
  unsigned long workSeconds;
  unsigned long secondsSinceWatering;
  bool lastWateringStatus;
  uint8_t _reserved;
};

void statInit();
void statUpdate();
unsigned long statGetWorkSeconds();
unsigned long statGetSecondsSinceWatering();
void statSetLastWateringStatus(bool pumpStatus);
bool statGetLastWateringStatus();

#endif