#include "CMMC_Modem.h"


CMMC_Modem::CMMC_Modem(Stream*)   {
}

void CMMC_Modem::configLoop() {
  yield();
}

void CMMC_Modem::configSetup() {
  yield();
}

void updateStatus(String s) {

}

void CMMC_Modem::setup() {
  nb = new CMMC_NB_IoT(this->_modemSerial);
  nb->setDebugStream(&Serial);
  nb->onDeviceReboot([]() {
    updateStatus(F("[user] Device rebooted."));
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
    updateStatus(buffer);
  });

  nb->onConnecting([]() {
    updateStatus("Attaching to NB-IoT...");
    delay(10);
  });

  static CMMC_Modem *that;
  that = this;
  nb->onConnected([]() {
    updateStatus("NB-IoT Connected.");
    Serial.print("[user] NB-IoT Network connected at (");
    Serial.print(millis());
    Serial.println("ms)");
    that->nb->createUdpSocket("103.20.205.85", 5683, UDPConfig::ENABLE_RECV);
    that->isNbConnected = 1;
  });
}

void CMMC_Modem::loop() {
}
