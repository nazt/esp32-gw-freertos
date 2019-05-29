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
};

struct shared_pool pool;

void showDate(const char* txt, const DateTime& dt) {
    mySerial.print(txt);
    mySerial.print("");
    mySerial.print(dt.year(), DEC);
    mySerial.print('/');
    mySerial.print(dt.month(), DEC);
    mySerial.print('/');
    mySerial.print(dt.day(), DEC);
    mySerial.print(' ');
    mySerial.print(dt.hour(), DEC);
    mySerial.print(':');
    mySerial.print(dt.minute(), DEC);
    mySerial.print(':');
    mySerial.print(dt.second(), DEC);
    // mySerial.print(" = ");
    // mySerial.print(dt.unixtime());
    // mySerial.print("s / ");
    // mySerial.print(dt.unixtime() / 86400L);
    // mySerial.print("d since 1970");
    mySerial.println();
}

static void task_serial1(void *parameter) {
    pool.pm10 = 0;
    pool.pm2_5 = 0;
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
      mySerial.printf("pool.pm10 = %f\r\n", pool.pm10);
      mySerial.printf("pool.pm2_5 = %f\r\n", pool.pm2_5);
      // mySerial.printf("pool.location = %s\r\n", pool.locationString.c_str());
      // mySerial.printf("pool.dt = ");
      // pool.printDt();
      vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

static void lcd_task(void *parameter) {
  static CMMC_LCD *lcd = new CMMC_LCD();
  lcd->setup();
  while(1) {
    lcd->pm10 = pool.pm10;
    lcd->pm2_5 = pool.pm2_5;
    lcd->loop();
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

static void tasks_init() {
  int priority = 1;
  xTaskCreate(task_serial1, "task_serial1", 4096, NULL, priority, NULL);
  // xTaskCreatePinnedToCore(task_serial1, "task_serial1", 4096, NULL, priority, NULL, 1);
  xTaskCreate(lcd_task, "lcd_task", 4096, NULL, 1, NULL);
}
