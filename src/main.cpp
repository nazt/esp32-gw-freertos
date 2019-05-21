
#include <CMMC_Legend.h>

// #include "modules/LCDModule.h"
// #include "modules/NTPModule.h"
// #include "modules/WiFiModule.h"
// #include "modules/MqttModule.h"
// #include "modules/SensorModule.h"

const char* MEOBOT_VERSION = "1.5";

CMMC_Legend os;
// LCDModule *lcdModule;
// NTPModule *ntpModule;
// WiFiModule* wifiModule;
// SensorModule *sensorModule;


void setup()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  delay(100);

  rtc = new CMMC_RTC();
  lcd = new CMMC_LCD();
  xTaskCreate(lcdTask, "lcdTask", 10000, /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    2,                /* Priority of the task. */
                    NULL);

  dustSensor = new CMMC_DustSensor(&Serial1);

  modules[0] = dustSensor;
  modules[1] = new CMMC_GPS(&Serial1);

  for (size_t i = 1; i < MODULE_SIZE; i++) {
    modules[i]->setup();
  }

  xTaskCreate(nbTask, "TaskTwo", 10000, /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);
}

void nbTask(void * parameter)
{
    NBSerial.begin(9600, SERIAL_8N1, 26 /*rx*/, 27 /* tx */);
    NBSerial.setTimeout(4);
    delay(20);
    modem = new CMMC_Modem(&NBSerial);
    modem->setup();
    while (1) {
      modem->loop();
      delay(10);
    }
    Serial.println("Ending task 2");
    vTaskDelete( NULL );
}

void lcdTask(void * parameter)
{
    lcd->setup();
    rtc->setup();
    int element;
    pinMode(EXT_WDT_PIN, OUTPUT);
    while (1) {
      rtc->loop();
      lcd->loop();
      if (modem == NULL) {
          digitalWrite(EXT_WDT_PIN, HIGH);
          delay(1);
          digitalWrite(EXT_WDT_PIN, LOW);
          continue;
      }

      uint32_t lastSentInSeconds = (millis() - modem->lastSentOkMillis)/1000;
      Serial.println(lastSentInSeconds);
      // if (modem->lastSentOkMillis == 0) {
      //     digitalWrite(EXT_WDT_PIN, HIGH);
      //     digitalWrite(EXT_WDT_PIN, LOW);
      // }
      if (modem->lastSentOkMillis > 0 && lastSentInSeconds > 30) {
            Serial.println("FFFF.. CAUSE EXTERNAL RST.");
            ESP.deepSleep(1e6);
            delay(100);
            ESP.restart();
            delay(10);
      }
      else {
        if (millis() > 300L*1000) {
            Serial.println("FFFF.. CAUSE EXTERNAL RST.");
            ESP.deepSleep(1e6);
            delay(100);
            ESP.restart();
            delay(10);
        }
        else {
          digitalWrite(EXT_WDT_PIN, HIGH);
          digitalWrite(EXT_WDT_PIN, LOW);
        }
      }

      // BaseType_t xStatus;
      // const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
      // xStatus = xQueueReceive( xQueueMain, &element, xTicksToWait );
      // /* check whether receiving is ok or not */
      // if(xStatus == pdPASS){
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      // }
    }
    Serial.println("Ending LCD Task");
    vTaskDelete( NULL );
}


void loop() {
  for (size_t i = 0; i < MODULE_SIZE; i++) {
    modules[i]->loop();
  }
  if (modem->lastSentOkMillis > 0) {
    uint32_t lastSentInSeconds = (millis() - modem->lastSentOkMillis)/1000;
    Serial.println(lastSentInSeconds);
    if (lastSentInSeconds > 60) {
      Serial.println("FFFF.. CAUSE EXTERNAL RST.");
      ESP.deepSleep(1e6);
      delay(100);
    }
  }

  // Serial.printf("rebootCount = %lu\r\n", rebootCount);
}


void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : Serial.println ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
    case 3 : Serial.println ("SW_RESET");break;               /**<3,  Software reset digital core*/
    case 4 : Serial.println ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : Serial.println ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : Serial.println ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : Serial.println ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : Serial.println ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : Serial.println ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : Serial.println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : Serial.println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : Serial.println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : Serial.println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : Serial.println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : Serial.println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : Serial.println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Serial.println ("NO_MEAN");
  }
  // lcdModule = new LCDModule();
  // wifiModule = new WiFiModule();
  // ntpModule = new NTPModule();
  // sensorModule = new SensorModule();
  // os.addModule(sensorModule);
  // os.addModule(lcdModule);
  // os.addModule(wifiModule);
  // os.addModule(ntpModule);
  // os.addModule(new MqttModule());

  os.setup();
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
  Serial.printf("MEOBOT VERSION: %s\r\n", MEOBOT_VERSION);
}

void loop()
{
  os.run();
}
