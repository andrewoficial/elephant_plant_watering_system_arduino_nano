#include "GroundWatering.h"
#include "config.h"
#include "StatisticAccumulator.h"
#include "StatusManager.h"
#include "GroundMeasurement.h"

static bool pumpRunning = false;
static unsigned long pumpStart = 0;
static unsigned long pumpDuration = 0;
static unsigned long pumpDurationEmercy = 11 * 1000UL;
static unsigned long pumpStartEmercy = 0;
static int groundHydmityDelta = 0;

enum WaterCheckState {
    WCHECK_IDLE,      // проверка не запущена
    WCHECK_WAITING,   // ожидание, можно вызвать checkWaterFlow()
    WCHECK_DONE       // проверка завершена
};

static WaterCheckState waterCheckState = WCHECK_IDLE;
static int waterCheckStartValue = 0;      // начальное значение АЦП
static int waterCheckEndValue = 0;        // конечное значение АЦП
static unsigned long waterCheckStartTime = 0; // когда был запомнен старт

void initWatering() {
    Serial.println(F("\n--- Инициализация модуля полива (Реле) ---"));
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);

    if (digitalRead(PUMP_PIN) == LOW) {
        Serial.println(F("✅ Реле помпы OK (по умолчанию ВЫКЛ)"));
    } else {
        Serial.println(F("⚠️ ВНИМАНИЕ: Реле помпы ВКЛЮЧЕНО при старте! Проверьте логику или обрыв."));
    }

    // Помпа гарантированно выключена, сбрасываем флаг работы
    statusSetFlag(EFLAG_PUMP_RUNNING, false);
    // Флаг ошибки реле пока не трогаем (можно выставить при реальной диагностике)
}

void startPump(unsigned int seconds) {
    if(statusGetFlag(EFLAG_PUMP_RUNNING)){
        Serial.println("Pump already ON");
        return;
    }
    if(statusGetFlag(EFLAG_MOTOR_EMERGENCY)){
        Serial.println("Pump blocked ");
        return;
    }
    
    // Запускаем обнаружение воды
    startWaterDetection();

    digitalWrite(PUMP_PIN, HIGH);
    pumpRunning = true;
    pumpStart = millis();
    pumpDuration = seconds * 1000UL;
    statSetLastWateringStatus(true);
    statusSetFlag(EFLAG_PUMP_RUNNING, true);

}

void stopPump() {
    digitalWrite(PUMP_PIN, LOW);
    pumpRunning = false;
    pumpDuration = 0;
    pumpStartEmercy = 0;
    statSetLastWateringStatus(false);
    statusSetFlag(EFLAG_PUMP_RUNNING, false);
    stopWaterDetection();   // сброс проверки
}

void updatePump() {
    if (pumpRunning && pumpDuration > 0) {
        if (millis() - pumpStart >= pumpDuration) {
            stopPump();
        }
        // Проверяем воду через pumpDurationEmercy (например 11 сек)
        if (pumpRunning && (millis() - pumpStart) >= pumpDurationEmercy) {
            if (!checkWaterFlow()) {
                // вода не пошла
                statusSetFlag(EFLAG_MOTOR_EMERGENCY, true);
                Serial.println("Water not detected! Emergency stop.");
                stopPump();
            }
        }
    } else if (pumpRunning) {
        // бесконечный режим – можно использовать pumpStartEmercy как таймер проверки
        if (pumpStartEmercy == 0) {
            pumpStartEmercy = millis();
        } else if (millis() - pumpStartEmercy >= pumpDurationEmercy) {
            if (!checkWaterFlow()) {
                statusSetFlag(EFLAG_MOTOR_EMERGENCY, true);
                Serial.println("Water not detected! Emergency stop (auto).");
                stopPump();
                pumpStartEmercy = 0;
            } else {
                // Вода идёт – можно продолжать бесконечно (или поставить повторную проверку)
                // Сбросим проверку, чтобы не висела в DONE
                waterCheckState = WCHECK_IDLE;
                pumpStartEmercy = 0;  // начнём новый цикл
            }
        }
    }
    
}

bool isPumpRunning() {
    return pumpRunning;
}

// Запустить проверку – запомнить текущую влажность
void startWaterDetection() {
    if (waterCheckState != WCHECK_IDLE) {
        // Уже запущено – игнорируем или можно сбросить
        return;
    }
    waterCheckStartValue = getSoilMoisture();
    waterCheckStartTime = millis();
    waterCheckEndValue = 0;
    waterCheckState = WCHECK_WAITING;
}

// Проверить, пошла ли вода (вызывать после задержки, например через 5–10 сек)
// Возвращает true, если изменение >= порога, false иначе.
// После вызова состояние переходит в DONE (или остаётся WAITING, если ещё рано).
bool checkWaterFlow() {
    if (waterCheckState != WCHECK_WAITING) {
        return false;   // неправильная последовательность
    }

    // Минимальное время ожидания (3 секунды, чтобы вода дошла)
    const unsigned long MIN_WAIT_MS = 3000;
    if (millis() - waterCheckStartTime < MIN_WAIT_MS) {
        // Ещё рано – вернём false, но состояние не меняем
        return false;
    }

    waterCheckEndValue = getSoilMoisture();
    int delta = waterCheckEndValue - waterCheckStartValue;
    waterCheckState = WCHECK_DONE;

    if (delta >= WATER_DETECTION_THRESHOLD) {
        return true;    // вода пошла
    } else {
        return false;   // сухо
    }
}

// Принудительно завершить проверку (если помпу выключили раньше)
void stopWaterDetection() {
    waterCheckState = WCHECK_IDLE;
}

// Для отладки: возвращает строку с результатами последней проверки
// Формат: "start=450 end=730 delta=280 OK"
void getWaterDetectionResult(char* buffer, size_t bufSize) {
    if (waterCheckState != WCHECK_DONE) {
        snprintf(buffer, bufSize, "No result");
        return;
    }
    int delta = waterCheckEndValue - waterCheckStartValue;
    snprintf(buffer, bufSize, "start=%d end=%d delta=%d %s",
             waterCheckStartValue, waterCheckEndValue, delta,
             (delta >= WATER_DETECTION_THRESHOLD) ? "OK" : "FAIL");
}

