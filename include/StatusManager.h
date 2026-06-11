#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include <Arduino.h>
#include "EEPROM_Manager.h"

// Биты флагов (строго соответствуют комментарию в EEPROM_Manager.h)
enum ErrorFlags : uint16_t {
    EFLAG_EEPROM_FAIL       = 0,  // Ошибка инициализации памяти
    EFLAG_HC12_FAIL         = 1,  // Ошибка подключения HC-12
    EFLAG_DHT_FAIL          = 2,  // Ошибка подключения DHT11
    EFLAG_RELAY_FAIL        = 3,  // Ошибка подключения Реле
    EFLAG_SOIL_SENSOR_FAIL  = 4,  // Ошибка подключения Датчика влажности
    EFLAG_MOTOR_EMERGENCY   = 5,  // Моторчик аварийно отключен (автоматика не включит)
    EFLAG_MOTOR_MANUAL_OFF  = 6,  // Моторчик отключен вручную (перезапустится по циклу)
    // 7 - резерв
    EFLAG_ADVRT_RECEIVED    = 8,  // Хотя бы раз получен ADVRT
    EFLAG_TEMP_SIMULATED    = 9,  // Температура имитируется (1) / настоящая (0)
    EFLAG_HUM_SIMULATED     = 10, // Влажность имитируется
    EFLAG_PRESS_SIMULATED   = 11, // Давление имитируется
    EFLAG_SOIL_SIMULATED    = 12, // Влажность почвы имитируется
    EFLAG_PUMP_RUNNING      = 13, // Помпа открыта (1) / закрыта (0)
    EFLAG_SOIL_POWER        = 14  // Питание датчика влажности подано (1) / снято (0)
    // 15 - резерв
};

void statusInit();
void statusSetFlag(ErrorFlags flag, bool value);
bool statusGetFlag(ErrorFlags flag);
void statusGetFlagsString(char* buffer);   // заполняет 16 символов '0'/'1'

#endif