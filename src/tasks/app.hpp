#include "tasks/dust/CMMC_DustSensor.h"
#include "tasks/gps/CMMC_GPS.h"
#include "tasks/rtc/CMMC_RTC.h"

static const int RX_BUF_SIZE = 1024;

void showDate(const char* txt, const DateTime& dt) {
    Serial.print(txt);
    Serial.print("");
    Serial.print(dt.year(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.day(), DEC);
    Serial.print(' ');
    Serial.print(dt.hour(), DEC);
    Serial.print(':');
    Serial.print(dt.minute(), DEC);
    Serial.print(':');
    Serial.print(dt.second(), DEC);

    Serial.print(" = ");
    Serial.print(dt.unixtime());
    Serial.print("s / ");
    Serial.print(dt.unixtime() / 86400L);
    Serial.print("d since 1970");

    Serial.println();
}

static void task_serial1(void *parameter) {
    static CMMC_DustSensor *dustSensor = new CMMC_DustSensor(&Serial1);
    static CMMC_GPS *gps = new CMMC_GPS(&Serial1);
    static CMMC_RTC *rtc = new CMMC_RTC();
    dustSensor->setup();
    gps->setup();
    rtc->setup();
    while(1) {
      dustSensor->loop();
      gps->loop();
      rtc->loop();
      showDate("[RTC]:", rtc->getDateTime());
      if (gps->_lastSyncRtc > 0) {
        showDate("[GPS]:", gps->getDateTime());
        rtc->adjust(gps->getDateTime());
      }
      else {
        Serial.println("Waiting for gps..");
      }
    }
}

void tasks_init() {
  int priority = 1;
  xTaskCreate(task_serial1, "task_serial1", 2048, NULL, priority, NULL);
}
