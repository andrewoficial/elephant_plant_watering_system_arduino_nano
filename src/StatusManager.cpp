#include "StatusManager.h"

void statusInit() {
    // Флаги уже загружены в gConfig.statusFlags при eeprom_init()
    Serial.println("Status init");
}

void statusSetFlag(ErrorFlags flag, bool value) {
    if (value)
        gConfig.statusFlags |= (1 << flag);
    else
        gConfig.statusFlags &= ~(1 << flag);
    eeprom_save();
}

bool statusGetFlag(ErrorFlags flag) {
    return (gConfig.statusFlags & (1 << flag)) != 0;
}

void statusGetFlagsString(char* buffer) {
    for (int i = 15; i >= 0; i--) {
        buffer[15-i] = (gConfig.statusFlags & (1 << i)) ? '1' : '0';
    }
    buffer[16] = 0;
}