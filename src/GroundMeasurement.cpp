#include "GroundMeasurement.h"
#include "config.h"
#include "StatusManager.h"
#include "EEPROM_Manager.h"

static bool soilOk = false;

void soilInit() {
  Serial.println(F("\n--- Инициализация датчика влажности почвы ---"));
  pinMode(SOIL_POWER_PIN, OUTPUT);
  digitalWrite(SOIL_POWER_PIN, LOW);
  soilOk = true;  
  
  // Тестовое измерение при старте
  digitalWrite(SOIL_POWER_PIN, HIGH);
  delay(50); // Ждем чуть дольше для стабилизации при первом запуске
  int raw = analogRead(SOIL_ANALOG_PIN);
  digitalWrite(SOIL_POWER_PIN, LOW);

  if (raw >= 200 && raw <= 900) {
    Serial.println(F("✅ Датчик почвы OK (подключен)"));
    Serial.print(F("   Сырое значение АЦП: "));
    Serial.println(raw);
    statusSetFlag(EFLAG_SOIL_SENSOR_FAIL, false);
    statusSetFlag(EFLAG_SOIL_SIMULATED, false);
  } else {
    Serial.println(F("⚠️ ВНИМАНИЕ: Датчик почвы вне диапазона (возможно, не подключен)"));
    Serial.print(F("   Сырое значение АЦП: "));
    Serial.println(raw);
    Serial.println(F("   Режим эмуляции: возврат значения 450"));
    statusSetFlag(EFLAG_SOIL_SENSOR_FAIL, true); 
    statusSetFlag(EFLAG_SOIL_SIMULATED, true); 
  }
}

int getSoilMoisture() {
  if (!soilOk) return 0;
  
  digitalWrite(SOIL_POWER_PIN, HIGH);
  delay(40);
  int raw = analogRead(SOIL_ANALOG_PIN);
  digitalWrite(SOIL_POWER_PIN, LOW);
  return raw;
}

int getSoilMoistureDelta() {
  static int groundValue = -1;
  static unsigned long groundTime = 0;
  int current = getSoilMoisture();
  if (groundValue == -1 || millis() - groundTime > (unsigned long)gConfig.groundDeltaTime * 1000UL) {
    groundValue = current;
    groundTime = millis();
  }
  return current - groundValue;
}