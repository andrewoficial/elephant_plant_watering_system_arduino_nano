#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------- Пины (приведены в соответствие с макетом) ----------
#define RX_PIN            2   // HC-12 TX -> Nano D2
#define TX_PIN            3   // Nano D3 -> HC-12 RX (через делитель)
#define HC12_SET_PIN      4   // HC-12 SET -> Nano D4
#define PUMP_PIN          8   // Реле помпы -> Nano D8 (изменено с 4)
#define SOIL_POWER_PIN    7   // Питание датчика почвы -> Nano D7 (изменено с 5)
#define SOIL_ANALOG_PIN   A0  // Аналоговый вход датчика почвы

#define DHTPIN            6   // DHT11/22 Data -> Nano D6
#define DHTTYPE           DHT11 // Укажите DHT22, если у вас он

// ---------- Размеры буферов ----------
#define SERIAL_BUF_SIZE   64
#define RADIO_BUF_SIZE    64

// ---------- Адрес широковещания ----------
#define BROADCAST_ADDR    0

// ---------- EEPROM-адреса ----------
#define EEPROM_MAGIC_ADDR 0
#define EEPROM_MAGIC      0xA5
#define EEPROM_CONFIG_ADDR 1

// ---------- Значения по умолчанию ----------
#define DEFAULT_ADDR         1
#define DEFAULT_SOFT_BAUD    9600
#define DEFAULT_USB_BAUD     9600
#define DEFAULT_PUMP_TIME    10
#define DEFAULT_SGDE         10
#define DEFAULT_SGTI         60

#define WATER_DETECTION_THRESHOLD  50

// Флаги ошибок по умолчанию (все сброшены)
#define DEFAULT_ERROR_FLAGS  0x0000

#endif