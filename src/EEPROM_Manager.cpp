#include "EEPROM_Manager.h"
#include "StatusManager.h"
#include <avr/eeprom.h>

Config gConfig;

void eeprom_init() {
  uint8_t magic = eeprom_read_byte((const uint8_t*)EEPROM_MAGIC_ADDR);
  if (magic == EEPROM_MAGIC) {
    eeprom_read_block(&gConfig, (const void*)EEPROM_CONFIG_ADDR, sizeof(Config));
    statusSetFlag(EFLAG_EEPROM_FAIL, false);
  } else {
    statusSetFlag(EFLAG_EEPROM_FAIL, true);
    gConfig.magic = EEPROM_MAGIC;
    gConfig.address = DEFAULT_ADDR;
    gConfig.softBaud = DEFAULT_SOFT_BAUD;
    gConfig.usbBaud = DEFAULT_USB_BAUD;
    gConfig.tempOffset = 0.0f;
    gConfig.hydrOffset = 0.0f;
    gConfig.presOffset = 0.0f;
    gConfig.groundDelta = DEFAULT_SGDE;
    gConfig.groundDeltaTime = DEFAULT_SGTI;
    gConfig.statusFlags = DEFAULT_ERROR_FLAGS;   // новое
    eeprom_save();
  }
}

void eeprom_save() {
  eeprom_update_block(&gConfig, (void*)EEPROM_CONFIG_ADDR, sizeof(Config));
  eeprom_update_byte((uint8_t*)EEPROM_MAGIC_ADDR, EEPROM_MAGIC);
}

void eeprom_reset() {
  gConfig.magic = EEPROM_MAGIC;
  gConfig.address = DEFAULT_ADDR;
  gConfig.softBaud = DEFAULT_SOFT_BAUD;
  gConfig.usbBaud = DEFAULT_USB_BAUD;
  gConfig.tempOffset = 0.0f;
  gConfig.hydrOffset = 0.0f;
  gConfig.presOffset = 0.0f;
  gConfig.groundDelta = DEFAULT_SGDE;
  gConfig.groundDeltaTime = DEFAULT_SGTI;
  gConfig.statusFlags = DEFAULT_ERROR_FLAGS;     // новое
  eeprom_save();
}