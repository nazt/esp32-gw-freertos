#include <Wire.h>
#include <RTClib.h>
#include "CMMC_Modem.h"

#include "coap.h"
#include "coap-helper.h"

extern char sta_mac[18];
extern char softap_mac[18];
// extern static QueueHandle_t xQueueMain;

IPAddress aisip = IPAddress(103, 20, 205, 85);
RTC_DATA_ATTR int rebootCount = -1;

CMMC_Modem::CMMC_Modem(Stream* s, HardwareSerial* hwSerial)   {
  this->_modemSerial = s;
  this->hwSerial = hwSerial;
}

void CMMC_Modem::configLoop() {
  yield();
}

void CMMC_Modem::configSetup() {
  yield();
}

void CMMC_Modem::updateStatus(String s) {
  this->hwSerial->println(s);
  this->status = s;
}

void CMMC_Modem::setup() {
  Serial.println("setup modem..");
  this->status = "Initializing Modem.";
  // pinMode(13, INPUT);
  // while(1) {
  //   Serial.println(digitalRead(13));
  // }

  // #define TRUE_NB_IOT_CONF 1
  #define AIS_NB_IOT_CONF 1

  #ifdef TRUE_NB_IOT_CONF
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  delay(1);
  digitalWrite(13, HIGH);
  #endif

  #ifdef AIS_NB_IOT_CONF
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(1);
  digitalWrite(13, LOW);
  #endif

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
      delay(100);
      ESP.restart();
      delay(10);
    }
  });

  nb->onConnected([](void * parameter ) {
    that->updateStatus("Connected.");
    that->hwSerial->print("[user] NB-IoT Network connected at (");
    that->hwSerial->print(millis());
    that->hwSerial->println("ms)");
    that->nb->createUdpSocket("103.20.205.85", 5683, UDPConfig::ENABLE_RECV);
    that->isNbConnected = 1;
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
  int rt = 0;
  uint8_t buffer[buflen];
  memcpy(buffer, text, buflen);
  while (true) {
    updateStatus("dispatching queue...");
    if (nb->sendMessageHex(buffer, buflen, 0)) {
      updateStatus(">> send ok.");
      lastSentOkMillis = millis();
      delay(100);
      break;
    }
    else {
      updateStatus(">> send failed.");
      if (++rt > 5) {
            ESP.deepSleep(1e6);
            delay(100);
            ESP.restart();
            delay(10);
        break;
      }
    }
    delay(200);
  }
}

String CMMC_Modem::getStatus() {
  return this->status;
}
