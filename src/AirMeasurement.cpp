#include "AirMeasurement.h"
#include "StatusManager.h"
#include "EEPROM_Manager.h"
#include "config.h"
#include "DHT.h"

static DHT dht(DHTPIN, DHTTYPE);
static bool dhtOk = false;

void airInit() {
  Serial.println(F("\n--- Инициализация датчика воздуха (DHT) ---"));
  dht.begin();
  
  // Даем датчику время на калибровку после подачи питания
  delay(1000); 

  // Пробуем сделать первое чтение
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println(F("⚠️ ОШИБКА: DHT датчик не найден или выдает NaN!"));
    Serial.println(F("   Проверьте подключение пина D6 и питание."));
    statusSetFlag(EFLAG_DHT_FAIL, true);
    statusSetFlag(EFLAG_TEMP_SIMULATED, true);
    statusSetFlag(EFLAG_HUM_SIMULATED, true);
  } else {
    Serial.println(F("✅ DHT датчик OK"));
    Serial.print(F("   Первое чтение: T="));
    Serial.print(t, 1);
    Serial.print(F("°C, H="));
    Serial.print(h, 1);
    Serial.println(F("%"));
    statusSetFlag(EFLAG_DHT_FAIL, false);
    statusSetFlag(EFLAG_TEMP_SIMULATED, false);
    statusSetFlag(EFLAG_HUM_SIMULATED, false);
  }
  
  statusSetFlag(EFLAG_PRESS_SIMULATED, true); // Давления нет, ставим флаг ошибки
}


float getAirTemperature() {
  if (statusGetFlag(EFLAG_DHT_FAIL)) return 0.0f;
  float t = dht.readTemperature();
  if (isnan(t)) {
    statusSetFlag(EFLAG_DHT_FAIL, true);
    statusSetFlag(EFLAG_TEMP_SIMULATED, true);
    return 0.0f;
  }
  statusSetFlag(EFLAG_DHT_FAIL, false);
  statusSetFlag(EFLAG_TEMP_SIMULATED, false);
  return t + gConfig.tempOffset;
}

float getAirHumidity() {
  if (statusGetFlag(EFLAG_DHT_FAIL)) return 0.0f;
  float h = dht.readHumidity();
  if (isnan(h)) {
    statusSetFlag(EFLAG_DHT_FAIL, true);
    statusSetFlag(EFLAG_HUM_SIMULATED, true);
    return 0.0f;
  }
  statusSetFlag(EFLAG_DHT_FAIL, false);
  statusSetFlag(EFLAG_HUM_SIMULATED, false);
  return h + gConfig.hydrOffset;
}

float getAirPressure() {
  return 0.0f + gConfig.presOffset; 
}