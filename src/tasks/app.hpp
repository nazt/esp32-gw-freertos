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
static CMMC_LCD *lcd;

void showDate(const char* txt, const DateTime& dt) {
    SERIAL0.print(txt);
    SERIAL0.print("");
    SERIAL0.print(dt.year(), DEC);
    SERIAL0.print('/');
    SERIAL0.print(dt.month(), DEC);
    SERIAL0.print('/');
    SERIAL0.print(dt.day(), DEC);
    SERIAL0.print(' ');
    SERIAL0.print(dt.hour(), DEC);
    SERIAL0.print(':');
    SERIAL0.print(dt.minute(), DEC);
    SERIAL0.print(':');
    SERIAL0.print(dt.second(), DEC);
    // SERIAL0.print(" = ");
    // SERIAL0.print(dt.unixtime());
    // SERIAL0.print("s / ");
    // SERIAL0.print(dt.unixtime() / 86400L);
    // SERIAL0.print("d since 1970");
    SERIAL0.println();
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
      vTaskDelay(500/portTICK_PERIOD_MS);
      if (xpage == LCD_CONFIG) {
        continue;
      }

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
      SERIAL0.printf("pool.pm10 = %f\r\n", pool.pm10);
      SERIAL0.printf("pool.pm2_5 = %f\r\n", pool.pm2_5);
      SERIAL0.printf("pool.location = %s\r\n", pool.locationString.c_str());
      SERIAL0.printf("pool.dt = ");
      // pool.printDt();
      showDate("", pool.dt);
    }
}

static void lcd_task(void *parameter) {
  lcd = new CMMC_LCD();
  lcd->setup();
  while(1) {
    lcd->pm10 = pool.pm10;
    lcd->pm2_5 = pool.pm2_5;
    lcd->loop();
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

static void tasks_init() {
  int priority = 1;
  xTaskCreate(task_serial1, "task_serial1", 8192, NULL, priority, NULL);
  xTaskCreate(lcd_task, "lcd_task", 4096, NULL, 1, NULL);
  // xTaskCreatePinnedToCore(task_serial1, "task_serial1", 2048, NULL, priority, NULL, 1);
}
