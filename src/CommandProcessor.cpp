#include "CommandProcessor.h"
#include "StatusManager.h"
#include <avr/pgmspace.h>

// ----- Список команд (хранится в PROGMEM для экономии RAM) -----
const char cmdList[] PROGMEM =
  "SREV?\nREBOOT\nLIST?\nRESET\nWBAUD?\nWBAUD x\nHBAUD?\nHBAUD x\nNADR?\nNADR x\n"
  "CERR?\nCERR RESET\nPUMP ON\nPUMP OFF\nPUMP x\nTERM?\nTERM x\nHYDM?\nHYDM x\n"
  "PRES?\nPRES x\nHYGR?\nHYGR DELTA\nSGDE?\nSGDE x\nSGTI?\nSGTI x\nADVRT\nPUMP TEST";

static RadioSendFunc g_RadioSend = nullptr;

/*
ToDo / Документация команд:
--------------------------
Команды обновлены по результатам тестирования:

SREV?     - Запрос версии прошивки (ОК)
REBOOT    - Программный сброс (Исправлено с REBOT)
LIST?     - Вывод списка команд (Исправлено: теперь выводит по слову на строку)
RESET     - Сброс настроек к заводским (С подтверждением 5 сек)

WBAUD?    - Запрос текущей скорости SoftwareSerial (HC-12)
WBAUD x   - Установка скорости (9600, 19200 и т.д.), требует REBOOT
HBAUD?    - Запрос текущей скорости Hardware Serial (USB)
HBAUD x   - Установка скорости USB, требует REBOOT

NADR?     - Запрос сетевого адреса
NADR x    - Установка адреса (1-99) (ОК)

CERR?     - Запрос текущих флагов ошибки (Исправлено: возвращает 16-битную строку 0 и 1)
CERR RESET- Сброс флагов ошибок (Добавлено подтверждение в течение 5 сек!)

PUMP ON   - Включить помпу бесконечно
PUMP OFF  - Выключить помпу
PUMP xxxxx- Включить помпу на ХХХХХ секунд (например, PUMP 00005)
PUMP TEST - Проверяет подачу воды (её определение)

TERM?     - Запрос температуры (ОК)
TERM x    - Калибровка температуры. СТРОГО: знак, 2 цифры, точка, 2 цифры. Пример: TERM +20.51 или TERM -01.05
HYDM?     - Запрос влажности воздуха (ОК)
HYDM x    - Калибровка влажности. СТРОГО: знак, 2 цифры, точка, 2 цифры. Пример: HYDM +45.00

PRES?     - Запрос давления (Заглушка, вернет 0)
PRES x    - Калибровка давления (Пока пропускаем)

HYGR?     - Запрос влажности почвы (Измеряет и возвращает сырые данные АЦП)
HYGR DELTA- Возвращает дельту влажности за интервал SGTI

SGDE?     - Запрос порога дельты влажности почвы
SGDE x    - Установка порога дельты
SGTI?     - Запрос интервала измерения дельты (секунды)
SGTI x    - Установка интервала измерения дельты

ADVRT     - Отправка полного пакета телеметрии (в USB и по радио дублируется)
*/

void setRadioSendFunc(RadioSendFunc func) {
  g_RadioSend = func;
}

// Сравнение строки из RAM со строкой в PROGMEM
static bool cmdEquals_P(const char* str, PGM_P pgm) {
  while (*str && pgm_read_byte(pgm)) {
    if (*str != pgm_read_byte(pgm)) return false;
    str++; pgm++;
  }
  return *str == 0 && pgm_read_byte(pgm) == 0;
}

// Вывод числа с плавающей точкой через OutputFunc
static void sendFloat(OutputFunc out, float val, int decimals = 2) {
  char buf[16];
  dtostrf(val, 6, decimals, buf);
  out(buf);
}

// Жесткая проверка формата калибровки: +DD.DD или -DD.DD
static bool isValidCalibration(const char* arg) {
  if (strlen(arg) != 6) return false; // Длина ровно 6
  if (arg[0] != '+' && arg[0] != '-') return false; // Обязательный знак
  if (!isdigit(arg[1]) || !isdigit(arg[2])) return false; // Две цифры до точки
  if (arg[3] != '.') return false; // Точка-разделитель
  if (!isdigit(arg[4]) || !isdigit(arg[5])) return false; // Две цифры после точки
  return true;
}

// =================== ОСНОВНОЙ ДИСПЕТЧЕР КОМАНД ===================
void processCommand(const char* command, const char* argument, OutputFunc out, CommandSource src) {
  // ========== СИСТЕМНЫЕ КОМАНДЫ ==========
  if (cmdEquals_P(command, PSTR("SREV?"))) {
    out("FW:v1.0 ");
  }
  else if (cmdEquals_P(command, PSTR("REBOOT"))) { // Исправлено с REBOT
    out("Rebooting... ");
    delay(100);
    asm volatile("jmp 0");
  }
  else if (cmdEquals_P(command, PSTR("LIST?"))) {
    // Выводим по слову, чтобы каждое было на новой строке в терминале
    PGM_P p = cmdList;
    char wordBuf[16];
    uint8_t wIdx = 0;
    while (true) {
      char c = pgm_read_byte(p++);
      if (c == '\n' || c == ' ' || c == 0) {
        if (wIdx > 0) {
          wordBuf[wIdx] = 0;
          out(wordBuf); // Функция out автоматически добавит перенос строки (usbOutput)
          wIdx = 0;
        }
        if (c == 0) break;
      } else {
        if (wIdx < sizeof(wordBuf) - 1) {
          wordBuf[wIdx++] = c;
        }
      }
    }
  }
  else if (cmdEquals_P(command, PSTR("RESET"))) {
    static bool resetConfirm = false;
    static unsigned long resetTimer = 0;
    if (!resetConfirm) {
      out("Send RESET again within 5s to confirm ");
      resetConfirm = true;
      resetTimer = millis();
    } else {
      if (millis() - resetTimer < 5000) {
        eeprom_reset();
        out("Settings reset to defaults ");
      } else {
        out("Confirmation timed out ");
      }
      resetConfirm = false;
    }
  }
  // ========== ТЕЛЕМЕТРИЯ ==========
  else if (cmdEquals_P(command, PSTR("ADVRT"))) {
    float temp = getAirTemperature();
    float hum  = getAirHumidity();
    float press = getAirPressure(); 
    int   soil = getSoilMoisture(); 

    int16_t itemp = (int16_t)(temp * 100);
    uint16_t ihum = (uint16_t)(hum * 10);
    uint16_t ipress = (uint16_t)(press * 10);
    uint16_t isoil = (uint16_t)(soil * 10);

    unsigned long workSec = statGetWorkSeconds();
    unsigned long totalDays = workSec / 86400UL;
    uint8_t wY = totalDays / 365;
    uint16_t wD = totalDays % 365;
    uint8_t wh = (workSec % 86400UL) / 3600;
    uint8_t wm = (workSec % 3600) / 60;
    uint8_t ws = workSec % 60;

    unsigned long lastWaterSec = statGetSecondsSinceWatering();
    bool lastWaterStatus = statGetLastWateringStatus();
    statSetLastWateringStatus(lastWaterSec);
    totalDays = lastWaterSec / 86400UL;
    uint8_t pY = totalDays / 365;
    uint16_t pD = totalDays % 365;
    uint8_t ph = (lastWaterSec % 86400UL) / 3600;
    uint8_t pm = (lastWaterSec % 3600) / 60;
    uint8_t ps = lastWaterSec % 60;

    char errFlags[17];
    statusGetFlagsString(errFlags);
    char advBuf[60];
    snprintf(advBuf, sizeof(advBuf), "%+05d%03u%04u%04u%02u%03u%02u%02u%02u%s%02u%03u%02u%02u%02u%01u",
             itemp, ihum, ipress, isoil,
             wY, wD, wh, wm, ws,
             errFlags,
             pY, pD, ph, pm, ps,
             lastWaterStatus ? 1 : 0);
    
    out(advBuf);

    if (src == SRC_USB && g_RadioSend != nullptr) {
      g_RadioSend(advBuf);
    }
  }
  // ========== НАСТРОЙКА СКОРОСТЕЙ ==========
  else if (cmdEquals_P(command, PSTR("WBAUD"))) {
    if (strcmp(argument, "?") == 0) {
      char buf[7];
      ltoa(gConfig.softBaud, buf, 10);
      out(buf);
    } else {
      long baud = atol(argument);
      if (baud > 0) {
        gConfig.softBaud = baud;
        eeprom_save();
        out("SoftSerial baud set, REBOOT to apply ");
      } else {
        out("ERR Invalid baud ");
      }
    }
  }
  else if (cmdEquals_P(command, PSTR("HBAUD"))) {
    if (strcmp(argument, "?") == 0) {
      char buf[7];
      ltoa(gConfig.usbBaud, buf, 10);
      out(buf);
    } else {
      long baud = atol(argument);
      if (baud > 0) {
        gConfig.usbBaud = baud;
        eeprom_save();
        out("USB baud set, REBOOT to apply ");
      } else {
        out("ERR Invalid baud ");
      }
    }
  }
  // ========== СЕТЕВОЙ АДРЕС ==========
  else if (cmdEquals_P(command, PSTR("NADR?"))) {
    char buf[6];
    itoa(gConfig.address, buf, 10);
    out(buf);
  }
  else if (cmdEquals_P(command, PSTR("NADR"))) {
    int addr = atoi(argument);
    if (addr >= 1 && addr <= 99) {
      gConfig.address = addr;
      eeprom_save();
      out("OK ");
    } else {
      out("ERR:bad addr (1-99) ");
    }
  }
  // ========== ФЛАГИ ОШИБОК ==========
  else if (cmdEquals_P(command, PSTR("CERR"))) {
    if (strcmp(argument, "?") == 0) {
      char errFlags[17];
      statusGetFlagsString(errFlags);
      out(errFlags);
    } 
    else if (strcmp(argument, "RESET") == 0) {
        static bool cerrConfirm = false;
        static unsigned long cerrTimer = 0;
        if (!cerrConfirm) {
            out("Send CERR RESET again within 5s to confirm ");
            cerrConfirm = true;
            cerrTimer = millis();
        } else {
            if (millis() - cerrTimer < 5000) {
                gConfig.statusFlags &= 0xFF00;   // сбросить только флаги ошибок (биты 0-7)
                eeprom_save();
                out("Error flags cleared ");
            } else {
                out("Confirmation timed out ");
            }
            cerrConfirm = false;
        }
    }else {
      out("UNKNOWN CMD ");
    }
  }
  // ========== УПРАВЛЕНИЕ ПОМПОЙ ==========
  else if (cmdEquals_P(command, PSTR("PUMP"))) {
    if (strcmp(argument, "ON") == 0) {
      startPump(0);      
      out("Pump ON ");
    } else if (strcmp(argument, "OFF") == 0) {
      stopPump();
      out("Pump OFF ");
    } else {
      int sec = atoi(argument);
      if (sec > 0) {
        startPump(sec);
        out("Pump started for ");
        out(argument);
        out("s ");
      } else {
        out("ERR:bad pump time ");
      }
    }
  }
  else if (cmdEquals_P(command, PSTR("PUMP")) && strcmp(argument, "TEST") == 0) {
    bool pumpRunning = statusGetFlag(EFLAG_PUMP_RUNNING);
    if (pumpRunning) {
        out("Pump already running ");
        return;
    }
    // Запускаем помпу на 10 секунд
    startPump(10);
    out("Testing water flow... ");
    // Активно ждём 10 секунд (с вызовом updatePump, чтобы работал таймер)
    unsigned long testStart = millis();
    while (pumpRunning && (millis() - testStart < 15000)) { // чуть больше на всякий случай
        updatePump();
        delay(100);
    }
    // После завершения выводим результат
    char buf[40];
    getWaterDetectionResult(buf, sizeof(buf));
    out(buf);
}
  // ========== ТЕМПЕРАТУРА ВОЗДУХА (DHT) ==========
  else if (cmdEquals_P(command, PSTR("TERM?"))) {
    float t = getAirTemperature();
    sendFloat(out, t);
  }
  else if (cmdEquals_P(command, PSTR("TERM"))) {
    if (!isValidCalibration(argument)) {
      out("ERR Format: TERM +20.51 ");
    } else {
      float realTemp = atof(argument);
      float current = getAirTemperature();
      float oldOfset = gConfig.tempOffset;
      
      gConfig.tempOffset = realTemp - current + oldOfset;
      eeprom_save();
      
      out("Offset set ");
    }
  }
  // ========== ВЛАЖНОСТЬ ВОЗДУХА (DHT) ==========
  else if (cmdEquals_P(command, PSTR("HYDM?"))) {
    float h = getAirHumidity();
    sendFloat(out, h);
  }
  else if (cmdEquals_P(command, PSTR("HYDM"))) {
    if (!isValidCalibration(argument)) {
      out("ERR Format: HYDM +45.00 ");
    } else {
      float realH = atof(argument);
      float current = getAirHumidity();
      gConfig.hydrOffset = realH - current;
      eeprom_save();
      out("Hydrometry offset set ");
    }
  }
  // ========== ДАВЛЕНИЕ (Заглушка) ==========
  else if (cmdEquals_P(command, PSTR("PRES?"))) {
    float p = getAirPressure();
    sendFloat(out, p, 1);
  }
  else if (cmdEquals_P(command, PSTR("PRES"))) {
    float realP = atof(argument);
    float current = getAirPressure();
    gConfig.presOffset = realP - current;
    eeprom_save();
    out("Pressure offset set ");
  }
  // ========== ВЛАЖНОСТЬ ПОЧВЫ ==========
  else if (cmdEquals_P(command, PSTR("HYGR"))) {
    if (strcmp(argument, "?") == 0) {
      char buf[6];
      itoa(getSoilMoisture(), buf, 10); 
      out(buf);
    }
    else if (strcmp(argument, "DELTA") == 0) {
      // Пока просто возвращает текущую дельту. 
      // Интервал дельты задается командой SGTI x
      int delta = getSoilMoistureDelta(); 
      char buf[10];
      itoa(delta, buf, 10);
      out(buf);
    } else {
      out("UNKNOWN CMD ");
    }
  }
  else if (cmdEquals_P(command, PSTR("SGDE"))) {
    if (strcmp(argument, "?") == 0) {
      char buf[7];
      itoa(gConfig.groundDelta, buf, 10);
      out(buf);
    } else {
      int val = atoi(argument);
      if (val >= 0) {
        gConfig.groundDelta = val;
        eeprom_save();
        out("OK ");
      }
    }
  }
  else if (cmdEquals_P(command, PSTR("SGTI"))) {
    if (strcmp(argument, "?") == 0) {
      char buf[7];
      itoa(gConfig.groundDeltaTime, buf, 10);
      out(buf);
    } else {
      int val = atoi(argument);
      if (val > 0) {
        gConfig.groundDeltaTime = val;
        eeprom_save();
        out("OK ");
      }
    }
  }
  // ========== НЕИЗВЕСТНАЯ КОМАНДА ==========
  else {
    out("UNKNOWN CMD ");
  }
}