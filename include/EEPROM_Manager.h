#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <stdint.h>
#include "config.h"

struct Config {
  uint8_t  magic;              // признак инициализации
  uint8_t  address;            // сетевой адрес 1..99
  uint32_t softBaud;           // скорость SoftwareSerial (HC-12)
  uint32_t usbBaud;            // скорость USB Serial
  float    tempOffset;         // коррекция температуры
  float    hydrOffset;         // коррекция влажности
  float    presOffset;         // коррекция давления
  int16_t  groundDelta;        // порог SGDE
  uint16_t groundDeltaTime;    // интервал SGTI (сек)
  uint16_t statusFlags;         // флаги ошибок/статуса (16 бит)
};
extern Config gConfig;

void eeprom_init();
void eeprom_save();
void eeprom_reset();

#endif