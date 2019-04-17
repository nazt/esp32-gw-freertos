#include "CMMC_Modem.h"
// #define AIS_TOKEN "12dee170-4a4b-11e9-96dd-9fb5d8a71344" // devstation
#define AIS_TOKEN "76d514f0-217f-11e9-a028-9771a15972bf" // nat-devstation
#include "coap.h"
#include "coap-helper.h"
RTC_DATA_ATTR int rebootCount = -1;
void sendPacket(uint8_t *text, int buflen);
enum {
  TYPE_KEEP_ALIVE = 1,
  TYPE_SENSOR_NODE
} DATA_COAP_TYPE;

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
  this->status = "Initializing Modem.";

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);

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
    counter = (counter + 1) % 3;
    // String t = "Attching";
    String t = "";
    for (size_t i = 0; i < counter; i++) {
      t += String(".");
    }
    that->updateStatus(t);
    delay(500);
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
  static CMMC_Modem *that;
  that = this;
  keepAliveInterval.every_ms(1000, []() {
    static char jsonBuffer[1024];
    int pArrIdx = 0;
    uint32_t lastSentOkMillis = 0;
    unsigned int ct = 1;
    static char msgId[5];
    char latC[20];
    char lngC[20];
    char latlngC[40];

    uint8_t currentSleepTimeMinuteByte = 30;
    uint32_t msAfterESPNowRecv = millis();

    IPAddress ip = IPAddress(103, 20, 205, 85);
    uint8_t _buffer[1300];
    float  batt;
    float  batt_raw;
    float  batt_percent;
    int analogValue;
    CMMC_Interval keepAliveInterval;

    bzero(_buffer, sizeof(_buffer));
    uint32_t uptime_s =  millis() / 1000;
    Serial.println("KEEP ALIVE INTERAL...");
    printf(">> CASE; keep alive..\n");
    sprintf(jsonBuffer, "{\"loc\":\"%s\",\"reset\":%d,\"type\":%d,\"uptime_s\":%lu,\"heap\":%lu,\"batt\":%s,\"ct\":%lu,\"sleep\":%lu,\"payload\":\"%s\"}", latlngC, rebootCount, TYPE_KEEP_ALIVE, uptime_s, ESP.getFreeHeap(), String(batt).c_str(), ct++, currentSleepTimeMinuteByte, "X");
    uint16_t buflen = generate(_buffer, ip, 5683, "NBIoT/" AIS_TOKEN, COAP_CON,
                               COAP_POST, NULL, 0, (uint8_t*) jsonBuffer, strlen(jsonBuffer));
    Serial.println(jsonBuffer);
    that->sendPacket((uint8_t*)_buffer, buflen);
  });

  // while (true) {
  // updateStatus("dispatching queue...");
  // if (nb.sendMessageHex(buffer, buflen, 0)) {
  //   updateStatus(">> [ais] socket0: send ok.");
  //   lastSentOkMillis = millis();
  //   nbSentOk++;
  //   delay(100);
  //   break;
  // }
  // else {
  //   updateStatus(">> [ais] socket0: send failed.");
  //   if (++rt > 5) {
  //     delay(100);
  //     ESP.deepSleep(1e6);
  //     delay(100);
  //     break;
  //   }
  // }
}

void CMMC_Modem::sendPacket(uint8_t *text, int buflen) {
  int rt = 0;
  uint8_t buffer[buflen];
  memcpy(buffer, text, buflen);
  // char buffer[buflen*2+1];
  // bzero(buffer, strlen(buffer));
  //  for (int x = 0; x < buflen*2; x+= 2) {
  //   sprintf(buffer[x], "%02x", text[x]);
  // }
  // Serial.printf("buflen=%d, ", buflen);
  // Serial.println(buffer);
  while (true) {
    updateStatus("dispatching queue...");
    if (nb->sendMessageHex(buffer, buflen, 0)) {
      updateStatus(">> [ais] socket0: send ok.");
      lastSentOkMillis = millis();
      // nbSentOk++;
      delay(100);
      break;
    }
    else {
      updateStatus(">> [ais] socket0: send failed.");
      if (++rt > 5) {
        delay(100);
        ESP.deepSleep(1e6);
        delay(100);
        break;
      }
    }
    delay(200);
  }
}

String CMMC_Modem::getStatus() {
  return this->status;
}
