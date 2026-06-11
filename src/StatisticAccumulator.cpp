#include "StatisticAccumulator.h"
#include <avr/eeprom.h>
#include "EEPROM_Manager.h"   // для sizeof(Config) и EEPROM_CONFIG_ADDR

#define EEPROM_STAT_ADDR  (EEPROM_CONFIG_ADDR + sizeof(Config))

static StatData stat;
static unsigned long lastSecondTick = 0;
static unsigned long lastEepromSave = 0;

void statInit() {

  eeprom_read_block(&stat, (void*)EEPROM_STAT_ADDR, sizeof(StatData));
  // Сброс, если EEPROM пустая (0xFF) ИЛИ значение нереалистично большое
  if (stat.workSeconds == 0xFFFFFFFF || stat.workSeconds > 5UL * 365 * 86400) {
    stat.workSeconds = 0;
    stat.secondsSinceWatering = 0;
    stat.lastWateringStatus = false;
    eeprom_update_block(&stat, (void*)EEPROM_STAT_ADDR, sizeof(StatData));
  }
  lastSecondTick = millis();
  lastEepromSave = millis();
}


void statUpdate() {
  if (millis() - lastSecondTick >= 1000) {
    lastSecondTick += 1000;
    stat.workSeconds++;
    stat.secondsSinceWatering++;
  }
  if (millis() - lastEepromSave >= 1800000UL) { // 30 минут
    lastEepromSave += 1800000UL;
    eeprom_update_block(&stat, (void*)EEPROM_STAT_ADDR, sizeof(StatData));
  }
}

unsigned long statGetWorkSeconds() {
  return stat.workSeconds;
}

unsigned long statGetSecondsSinceWatering() {
  return stat.secondsSinceWatering;
}

void statSetLastWateringStatus(bool pumpStatus) {
  stat.lastWateringStatus = pumpStatus;
  if (!pumpStatus) {
    stat.secondsSinceWatering = 0;
  }
  eeprom_update_block(&stat, (void*)EEPROM_STAT_ADDR, sizeof(StatData));
}

bool statGetLastWateringStatus() {
  return stat.lastWateringStatus;
}