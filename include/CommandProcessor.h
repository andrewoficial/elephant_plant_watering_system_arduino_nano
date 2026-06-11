#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <Arduino.h>
#include "config.h"
#include "EEPROM_Manager.h"
#include "AirMeasurement.h"
#include "GroundMeasurement.h"
#include "GroundWatering.h"
#include "StatisticAccumulator.h"

// Тип функции вывода (USB или радио)
typedef void (*OutputFunc)(const char*);

// Источник команды
enum CommandSource { SRC_USB, SRC_RADIO };

// Функция обратного вызова для принудительной отправки по радио
typedef void (*RadioSendFunc)(const char* payload);

// Установка коллбека для радио
void setRadioSendFunc(RadioSendFunc func);

// Основной обработчик команд
void processCommand(const char* command, const char* argument, OutputFunc out, CommandSource src);

// Список команд в PROGMEM для команды LIST?
extern const char cmdList[] PROGMEM;

#endif