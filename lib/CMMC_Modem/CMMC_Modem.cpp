#include <Wire.h>
#include <RTClib.h>
#include "CMMC_Modem.h"
#include "CMMC_DustSensor.h"
#include <CMMC_RTC.h>

// #define AIS_TOKEN "12dee170-4a4b-11e9-96dd-9fb5d8a71344" // devstation
#define AIS_TOKEN "76d514f0-217f-11e9-a028-9771a15972bf" // nat-devstation
#define DUSTBOY2_1 "4f9b9660-62da-11e9-96dd-9fb5d8a71344"
#define DUSTBOY2_2 "531c0a90-62da-11e9-96dd-9fb5d8a71344"
#define DUSTBOY2_3 "5bc43a50-62da-11e9-96dd-9fb5d8a71344"
#define DUSTBOY2_4 "5fc17fa0-62da-11e9-96dd-9fb5d8a71344"
#define DUSTBOY2_5 "6229a600-62da-11e9-96dd-9fb5d8a71344"

#define DUSTBOY_ID DUSTBOY2_5

#include "coap.h"
#include "coap-helper.h"
static QueueHandle_t xQueueMain;


IPAddress aisip = IPAddress(103, 20, 205, 85);
RTC_DATA_ATTR int rebootCount = -1;

extern CMMC_DustSensor *dustSensor;
extern CMMC_GPS *gps;
extern CMMC_RTC *rtc;

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
  // pinMode(13, INPUT);
  // while(1) {
  //   Serial.println(digitalRead(13));
  // }

  #define TRUE_NB_IOT_CONF 1
  // #define AIS_NB_IOT_CONF 1

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
    delay(500);
    if (millis() - prev > (180 * 1000)) {
        ESP.deepSleep(1e6);
        delay(100);
    }
  });

  nb->onConnected([](void * parameter ) {
    that->updateStatus("Connected.");
    Serial.print("[user] NB-IoT Network connected at (");
    Serial.print(millis());
    Serial.println("ms)");
    that->nb->createUdpSocket("103.20.205.85", 5683, UDPConfig::ENABLE_RECV);
    that->isNbConnected = 1;
    // BaseType_t xStatus;
    // /* time to block the task until the queue has free space */
    // const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    // /* create data to send */
    // int element = 1;
    // /* send data to front of the queue */
    // xStatus = xQueueSendToFront( xQueueMain, &element, xTicksToWait );
  });

  nb->hello();
  nb->rebootModule();
  this->xQueue = xQueueCreate(20, sizeof(Data));
  xTaskCreate([&](void * parameter) -> void {
    /* keep the status of receiving data */
    BaseType_t xStatus;
    // /* time to block the task until data is available */
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    Data data;

    static char jsonBuffer[1024];
    uint8_t _buffer[2000];

    for (;;) {
      /* receive data from the queue */
      // if (!that->isNbConnected) {
      //   continue;
      // }
      xStatus = xQueueReceive( that->xQueue, &data, xTicksToWait );
      bzero(_buffer, sizeof(_buffer));
      if (xStatus == pdPASS) {
        /* print the data to terminal */
        Serial.print("receiveTask got data: ");
        Serial.print("packet type = ");
        Serial.println(data.packet_type);
        if (data.packet_type == TYPE_KEEP_ALIVE) {
          Serial.println(">>> TYPE_KEEP_ALIVE");
          sprintf(jsonBuffer, "{\"ap\": \"%s\", \"pm10\":%s,\"pm2_5\":%s,\"loc\":\"%s\",\"reset\":%d,\"type\":%d,\"uptime_s\":%lu,\"unixtime\":%lu,\"heap\":%lu,\"batt\":%s,\"ct\":%lu,\"sleep\":%lu,\"payload\":\"%s\"}", softap_mac, String(data.pm10).c_str(), String(data.pm2_5).c_str(), data.latlngC, data.rebootCount, TYPE_KEEP_ALIVE, data.uptime_s, data.unixtime,  ESP.getFreeHeap(), String(data.batt).c_str(), data.ct++, 0, "X");

          Serial.printf("jsonBuffer= %s\r\n", jsonBuffer);
          // Serial.printf("   buffer = %s\r\n", _buffer);
          // DUSTBOY2_1
          uint16_t buflen = generate(_buffer, aisip, 5683, ("NBIoT/" DUSTBOY_ID),
          COAP_CON, COAP_POST, NULL, 0, (uint8_t*) jsonBuffer, strlen(jsonBuffer));
          Serial.printf("      len = %d\r\n", buflen);
          that->sendPacket((uint8_t*)_buffer, buflen);
        }
      }
      else {
        // Serial.println("QQQQ");
      }
    }
    vTaskDelete( NULL );
  },           /* Task function. */
  "receiveTask",        /* name of task. */
  10000,                    /* Stack size of task */
  NULL,                     /* parameter of the task */
  1,                        /* priority of the task */
  NULL);                    /* Task handle to keep track of created task */

}

void CMMC_Modem::loop() {
  nb->loop();
  static CMMC_Modem *that;
  that = this;
  keepAliveInterval.every_ms(10*1000, []() {
    Serial.println("KEEP ALIVE INTERAL...");
    printf(">> CASE; keep alive..\n");
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(300);
    // String(dustSensor->getPMValue(DustPM2_5)));
    Data data;
    data.packet_type = TYPE_KEEP_ALIVE;
    data.ct = that->nbSentCounter++;
    data.pm10 = dustSensor->getPMValue(DustPM10);
    data.pm2_5 = dustSensor->getPMValue(DustPM2_5);
    data.uptime_s = millis() / 1000;
    data.unixtime = rtc->getCurrentTimestamp();
    strcpy(data.latlngC, gps->getLocation().c_str());
    Serial.println("> sendTask2 is sending data");

    if (!that->isNbConnected) {
      Serial.println("NB IoT is not connected! skipped.");
      return;
    }
    else {
      xStatus = xQueueSendToFront(that->xQueue, &data, xTicksToWait);
      if ( xStatus == pdPASS ) {
        Serial.println("ENQUEUE!!!");
      }
      else {
        Serial.println("FAIL TO ENQUEUE.");
      }
    }
    // BaseType_t xStatusMain;
    // /* time to block the task until the queue has free space */
    // const TickType_t xTicksToWaitMain = pdMS_TO_TICKS(100);
    // /* create data to send */
    // int element = 1;
    // /* send data to front of the queue */
    // xStatusMain = xQueueSendToFront( xQueueMain, &element, xTicksToWaitMain );
  });
}

void CMMC_Modem::sendPacket(uint8_t *text, int buflen) {
  if (!isNbConnected) {
    Serial.println("NB IoT is not connected! skipped.");
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
