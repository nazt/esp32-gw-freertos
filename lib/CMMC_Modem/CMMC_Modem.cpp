#include "CMMC_Modem.h"


CMMC_Modem::CMMC_Modem(Stream* s)   {
  this->_modemSerial = s;
}

void CMMC_Modem::configLoop() {
  yield();
}

void CMMC_Modem::configSetup() {
  yield();
}

void CMMC_Modem::updateStatus(String s) {
  Serial.println(s);
  this->status = s;
}

void CMMC_Modem::setup() {
  Serial.println("setup modem..");
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);
  nb = new CMMC_NB_IoT(this->_modemSerial);
  static CMMC_Modem *that;
  that = this;
  nb->setDebugStream(&Serial);
  nb->onDeviceReboot([]() {
    that->updateStatus(F("[user] Device rebooted."));
  });

  nb->onDeviceReady([]() {
    Serial.println("[user] Device Ready!");
  });

  nb->onDeviceInfo([](CMMC_NB_IoT::DeviceInfo device) {
    Serial.print(F("# Module IMEI-->  "));
    Serial.println(device.imei);
    Serial.print(F("# Firmware ver-->  "));
    Serial.println(device.firmware);
    Serial.print(F("# IMSI SIM-->  "));
    Serial.println(device.imsi);
  });

  nb->onMessageArrived([](char *text, size_t len, uint8_t socketId, char* ip, uint16_t port) {
    char buffer[100];
    sprintf(buffer, "++ [recv:] socketId=%u, ip=%s, port=%u, len=%d bytes (%lums)", socketId, ip, port, len, millis());
    that->updateStatus(buffer);
  });

  static int counter;
  counter = 0;
  nb->onConnecting([]() {
    counter = (counter+1) % 3;
    // String t = "Attching";
    String t = "";
    for (size_t i = 0; i < counter; i++) {
      t+= String(".");
    }
    that->updateStatus(t);
    delay(100);
  });

  nb->onConnected([]() {
    that->updateStatus("Connected.");
    Serial.print("[user] NB-IoT Network connected at (");
    Serial.print(millis());
    Serial.println("ms)");
    that->nb->createUdpSocket("103.20.205.85", 5683, UDPConfig::ENABLE_RECV);
    that->isNbConnected = 1;
  });

  nb->hello();
  nb->rebootModule();
}

void CMMC_Modem::loop() {
  nb->loop();
}

String CMMC_Modem::getStatus() {
  return this->status;
}
