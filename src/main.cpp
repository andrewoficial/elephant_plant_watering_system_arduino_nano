#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "EEPROM_Manager.h"
#include "CommandProcessor.h"
#include "RadioProtocol.h"
#include "AirMeasurement.h"
#include "GroundMeasurement.h"
#include "GroundWatering.h"
#include "StatisticAccumulator.h"
#include "StatusManager.h"

SoftwareSerial radioSerial(RX_PIN, TX_PIN);
RadioProtocol radioProto(radioSerial);

char serialBuf[SERIAL_BUF_SIZE];
uint8_t serialIdx = 0;


static void uartOutputStatic(const char* text);
static void uartOutputStatic(const char* txt) {
  Serial.print(txt);
  Serial.println();
}

void radioSendWrapper(const char* payload) {
  radioProto.sendCustomPacket(payload);
}

// ========== Вспомогательная функция для AT-команд HC-12 ==========
String sendHC12AT(SoftwareSerial &ss, const char* cmd, unsigned long timeout = 1000) {
  ss.print(cmd);
  ss.print("\r\n");
  unsigned long start = millis();
  String resp = "";
  while (millis() - start < timeout) {
    while (ss.available()) {
      resp += (char)ss.read();
    }
  }
  resp.trim();
  return resp;
}

// ========== Полная инициализация HC-12 ==========
void initHC12() {
  Serial.println(F("\n--- Инициализация HC-12 ---"));
  pinMode(HC12_SET_PIN, OUTPUT);
  
  // 1. Переводим в режим команд (SET = LOW)
  digitalWrite(HC12_SET_PIN, LOW);
  delay(100);
  
  // Перезапускаем порт для очистки буфера
  radioSerial.end();
  delay(50);
  radioSerial.begin(9600);
  delay(50);
  
  // Очищаем буфер приема
  while(radioSerial.available()) radioSerial.read();
  
  // 2. Запрашиваем версию прошивки
  String ver = sendHC12AT(radioSerial, "AT+V");
  if (ver.length() > 0) {
    Serial.print(F("HC-12 Version: "));
    Serial.println(ver);
  } else {
    Serial.println(F("⚠️ HC-12 не ответил на AT+V (нет питания или не тот пин SET)"));
  }
  
  // Можно запросить все параметры
  String params = sendHC12AT(radioSerial, "AT+RX");
  if (params.length() > 0) {
    Serial.print(F("HC-12 Params: "));
    Serial.println(params);
  }
  
  // 3. Переводим в прозрачный режим (SET = HIGH)
  digitalWrite(HC12_SET_PIN, HIGH);
  delay(100);
  
  // 4. Проверяем, что модуль действительно вышел из AT-режима
  String checkResp = sendHC12AT(radioSerial, "AT", 500);
  
  if (checkResp.indexOf("OK") != -1) {
    Serial.println(F("⚠️ ВНИМАНИЕ: HC-12 остался в AT-режиме! Ответил OK на AT-команду."));
    Serial.println(F("Попытка принудительного перевода..."));
    digitalWrite(HC12_SET_PIN, LOW);
    delay(100);
    sendHC12AT(radioSerial, "AT"); 
    digitalWrite(HC12_SET_PIN, HIGH);
    delay(100);
  } else {
    Serial.println(F("✅ HC-12 успешно перешел в прозрачный режим (на AT не отвечает)"));
  }
  
  // 5. Финальная очистка буфера от мусора/эха при переключении режимов
  while(radioSerial.available()) radioSerial.read();
}

void processSerialInput() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (serialIdx > 0) {
        serialBuf[serialIdx] = 0;
        char* cmd = serialBuf;
        char* arg = nullptr;
        for (uint8_t i = 0; serialBuf[i]; i++) {
          if (serialBuf[i] == ' ') {
            serialBuf[i] = 0;
            arg = serialBuf + i + 1;
            break;
          }
        }
        processCommand(cmd, arg ? arg : "", uartOutputStatic, SRC_USB);
        serialIdx = 0;
      }
    } else {
      if (serialIdx < SERIAL_BUF_SIZE - 1) {
        serialBuf[serialIdx++] = c;
      }
    }
  }
}

void setup() {
  // Сначала запускаем USB порт для вывода
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println(F("\n\n========================================"));
  Serial.println(F("  СТАРТ СИСТЕМЫ АВТОПОЛИВА (Arduino Nano)"));
  Serial.println(F("========================================"));

  // Инициализация EEPROM и статистики
  eeprom_init();        
  statInit();
  statusInit();

  // Инициализация радиомодуля (с полной проверкой)
  initHC12();

  // Инициализация железа (датчики, реле)
  initWatering();
  airInit();
  soilInit();

  setRadioSendFunc(radioSendWrapper);

  Serial.println(F("\n✅ Система полностью готова к работе."));
  Serial.println(F("Ожидание команд по USB или Радио...\n"));
}

void loop() {
  processSerialInput();
  radioProto.processInput();
  statUpdate();
  updatePump();

}