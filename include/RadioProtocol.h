#ifndef RADIO_PROTOCOL_H
#define RADIO_PROTOCOL_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "CommandProcessor.h"

class RadioProtocol {
  char buf[RADIO_BUF_SIZE];
  uint8_t idx;
  char responseBuf[RADIO_BUF_SIZE];
  uint8_t responseLen;
  SoftwareSerial &radio;
  static RadioProtocol* instance;

public:
  RadioProtocol(SoftwareSerial &ss) : radio(ss), idx(0), responseLen(0) {}

  void processInput();
  void sendCustomPacket(const char* payload);

private:
  void parsePacket(const char* msg);
  static void radioOutputStatic(const char* text);
  void sendResponse();
  uint16_t crc16(const uint8_t* data, uint8_t len);
};

#endif