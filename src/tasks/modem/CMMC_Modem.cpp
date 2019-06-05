#include <Wire.h>
#include <RTClib.h>
#include "CMMC_Modem.h"

extern char sta_mac[18];
extern char softap_mac[18];
// extern static QueueHandle_t xQueueMain;

IPAddress aisip = IPAddress(103, 20, 205, 85);
RTC_DATA_ATTR int rebootCount = -1;

CMMC_Modem::CMMC_Modem(Stream* s, HardwareSerial* hwSerial, MODEM_TYPE modem_type)   {
  this->_modemSerial = s;
  this->hwSerial = hwSerial;
  this->_modemType = modem_type;
}

void CMMC_Modem::configLoop() {
  yield();
}

void CMMC_Modem::configSetup() {
  yield();
}

void CMMC_Modem::updateStatus(String s) {
  this->hwSerial->println(s);
  strcpy(this->status, s.c_str());
}

void CMMC_Modem::setup() {
  Serial.println("setup modem..");
  // this->status = "Initializing Modem.";
  strcpy(this->status, "Initializing Modem.");



  if (this->_modemType == TYPE_AIS_NB_IOT) {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1);
    digitalWrite(13, LOW);
  }
  else if (this->_modemType == TYPE_TRUE_NB_IOT) {
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    delay(1);
    digitalWrite(13, HIGH);
  }
  else {
    this->hwSerial->println("INVALID MODEM TYPE CONFIG.");
    this->hwSerial->println("INVALID MODEM TYPE CONFIG.");
    this->hwSerial->println("INVALID MODEM TYPE CONFIG.");
    this->hwSerial->println("INVALID MODEM TYPE CONFIG.");
    this->hwSerial->println("INVALID MODEM TYPE CONFIG.");
  }

  Serial.println("Initializing CMMC NB-IoT");
  nb = new CMMC_NB_IoT(this->_modemSerial);
  static CMMC_Modem *that;
  that = this;
  nb->setDebugStream(&Serial);
  nb->onDeviceReboot([]() {
    that->updateStatus(F("[user] Device rebooted."));
    delay(100);
  });

  nb->onDeviceReady([]() {
    that->hwSerial->println("[user] Device Ready!");
  });

  nb->onDeviceInfo([](CMMC_NB_IoT::DeviceInfo device) {
    that->hwSerial->print(F("# Module IMEI-->  "));
    that->hwSerial->println(device.imei);
    that->hwSerial->print(F("# Firmware ver-->  "));
    that->hwSerial->println(device.firmware);
    that->hwSerial->print(F("# IMSI SIM-->  "));
    that->hwSerial->println(device.imsi);
  });

  nb->onMessageArrived([](char *text, size_t len, uint8_t socketId, char* ip, uint16_t port) {
    char buffer[100];
    sprintf(buffer, "++ [recv:] socketId=%u, ip=%s, port=%u, len=%d bytes (%lums)", socketId, ip, port, len, millis());
    that->updateStatus(buffer);
  });

  static int counter;
  static uint32_t prev;
  counter = 0;
  prev = millis();
  nb->onConnecting([]() {
    counter = (counter + 1);
    // String t = "Attching";

    String t = "";
    for (size_t i = 0; i <= counter%3; i++) {
      t += String(".");
    }
    that->updateStatus(t);

    vTaskDelay(500 / portTICK_PERIOD_MS);
    if (millis() - prev > (180 * 1000)) {
      ESP.deepSleep(1e6);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ESP.restart();
    }
  });

  nb->onConnected([](void * parameter ) {
    that->updateStatus("Connected.");
    that->hwSerial->print("[user] NB-IoT Network connected at (");
    that->hwSerial->print(millis());
    that->hwSerial->println("ms)");
    that->nb->createUdpSocket("103.20.205.85", 5683, UDPConfig::ENABLE_RECV);
    that->isNbConnected = 1;
    that->_locked = false;
  });

  nb->hello();
  nb->rebootModule();
}

void CMMC_Modem::loop() {
  nb->loop();
  static CMMC_Modem *that;
  that = this;
}

void CMMC_Modem::sendPacket(uint8_t *text, int buflen) {
  if (!isNbConnected) {
    this->hwSerial->println("NB IoT is not connected! skipped.");
    return;
  }
  this->_locked = true;
  int rt = 0;
  uint8_t buffer[buflen];
  memcpy(buffer, text, buflen);
  while (true) {
    updateStatus("dispatching queue...");
    if (nb->sendMessageHex(buffer, buflen, 0)) {
      lastSentOkMillis = millis();
      // vTaskDelay(500/portTICK_PERIOD_MS);
      updateStatus("send ok.");
      delay(500);
      break;
    }
    else {
      updateStatus(">> send failed.");
      if (++rt > 5) {
            ESP.deepSleep(1e6);
            vTaskDelay(200/portTICK_PERIOD_MS);
            ESP.restart();
            delay(10);
        break;
      }
    }
    // vTaskDelay(200 / portTICK_PERIOD_MS);
    delay(200);
  }
  this->_locked = false;
}

String CMMC_Modem::getStatus() {
  return String(this->status);
}

bool CMMC_Modem::isLocked() {
  return this->_locked;
}
