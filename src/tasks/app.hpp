#include "tasks/dust/CMMC_DustSensor.h"
#include "tasks/gps/CMMC_GPS.h"
#include "tasks/rtc/CMMC_RTC.h"
#include "tasks/lcd/CMMC_LCD.h"
#include <TinyGPS++.h>

static const int RX_BUF_SIZE = 1024;
struct shared_pool {
  float pm10;
  float pm2_5;
  DateTime dt;
  TinyGPSLocation location;
  String locationString;
  void printDt() {
    Serial.print("")  ;
    Serial.printf("%2d", dt.year());
    Serial.printf("%2d", dt.month());
    Serial.print('/');
    Serial.printf("%2d", dt.day());
    Serial.print(' ');
    Serial.printf("%2d", dt.hour());
    Serial.print(':');
    Serial.printf("%2d", dt.minute());
    Serial.print(':');
    Serial.printf("%2d", dt.second());
    Serial.println();
  }
};

struct shared_pool pool;

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
    // Serial.print(" = ");
    // Serial.print(dt.unixtime());
    // Serial.print("s / ");
    // Serial.print(dt.unixtime() / 86400L);
    // Serial.print("d since 1970");
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
      pool.pm10 = dustSensor->getPMValue(DustPM10);
      pool.pm2_5 = dustSensor->getPMValue(DustPM2_5);
      pool.dt = rtc->getDateTime();
      // pool.location = gps->getLocation();
      // showDate("[RTC]:", pool.location);
      // showDate("[GPS]:", pool.location);
      pool.locationString = gps->getLocationString();
      if (gps->_lastSyncRtc > 0) {
        rtc->adjust(gps->getDateTime());
      }
      else {
      }
      Serial.printf("pool.pm10 = %f\r\n", pool.pm10);
      Serial.printf("pool.pm2_5 = %f\r\n", pool.pm2_5);
      Serial.printf("pool.location = %s\r\n", pool.locationString.c_str());
      Serial.printf("pool.dt = ");
      pool.printDt();
    }
}

static void lcd_task(void *parameter) {
  // os.addModule(new CMMC_LCD());
  static CMMC_LCD *lcd = new CMMC_LCD();
  lcd->setup();
  // lcd->hello();
  while(1) {
    lcd->loop();
  }
}

void tasks_init() {
  int priority = 1;
  // xTaskCreate(task_serial1, "task_serial1", 4096, NULL, priority, NULL);
  // xTaskCreatePinnedToCore(task_serial1, "task_serial1", 4096, NULL, priority, NULL, 1);
  // xTaskCreatePinnedToCore(lcd_task, "lcd_task", 4096, NULL, 2, NULL, 1);
}
