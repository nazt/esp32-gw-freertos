#include "tasks/dust/CMMC_DustSensor.h"
#include "tasks/gps/CMMC_GPS.h"
static const int RX_BUF_SIZE = 1024;

void showDate(const char* txt, const DateTime& dt) {
    Serial.print(txt);
    Serial.print(' ');
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

static void task_serial1(void * parameter) {
    static CMMC_DustSensor *dustSensor = new CMMC_DustSensor(&Serial1);
    static CMMC_GPS *gps = new CMMC_GPS(&Serial1);
    dustSensor->setup();
    gps->setup();
    while(1) {
      dustSensor->loop();
      gps->loop();
      showDate("", gps->getDateTime());
      Serial.printf("loc: %s\r\n", gps->getLocation().c_str());
      // Serial.printf("satellites=%lu\r\n", gps->gps.satellites.value());
    }
}

void tasks_init() {
  int priority = 1;
  xTaskCreate(task_serial1, "task_serial1", 2048, NULL, priority, NULL);
}
