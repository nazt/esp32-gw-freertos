#include "DustModule.h"
extern HardwareSerial SERIAL0;

// extern LCDModule* lcdModule;
DustModule::DustModule(HardwareSerial *s) {
    pms = new PMS(*s);
}

void DustModule::isLongPressed() {
}

void DustModule::configLoop() {
}

void DustModule::configSetup() {

}

void DustModule::config(CMMC_System *os, AsyncWebServer *server) {
}

void DustModule::configWebServer()
{
  static DustModule *that = this;
}

void DustModule::setup()
{
      pms->wakeUp();
      delay(500);
      pms->activeMode();

      data.PM_SP_UG_1_0 = 0;
      data.PM_SP_UG_2_5 = 0;
      data.PM_SP_UG_10_0 = 0;
      data.PM_AE_UG_1_0 = 0;
      data.PM_AE_UG_2_5 = 0;
      data.PM_AE_UG_10_0 = 0;

      _pm2_5 = _pm2_5_bucket.add(0.0);
      _pm10 = _pm10_bucket.add(0.0);
      SERIAL0.println("---------------");
      SERIAL0.print("pm2.5> ");
      SERIAL0.println(_pm2_5);
      SERIAL0.print("pm10> ");
      SERIAL0.println(_pm2_5);
      SERIAL0.println("---------------");

}

float DustModule::getPm10() {
  return _pm10;
  // return _pm10_bucket.get();
}

float DustModule::getPm2_5() {
  return _pm2_5;
  // return _pm10_bucket.get();
}

void DustModule::loop() {
    Serial1.begin(9600, SERIAL_8N1, 32 /*rx*/, 33 /* tx */);
    pms = new PMS(Serial1);
  // ti.every_ms(1, [&]() {
    if (pms->read(data)) {
      _invalid_count = 0;
      // SERIAL0.println();
      // SERIAL0.print("PM 1.0 (ug/m3): ");
      // SERIAL0.println(data.PM_AE_UG_1_0);
      //
      SERIAL0.print("PM 2.5 (ug/m3): ");
      SERIAL0.println(data.PM_AE_UG_2_5);
      SERIAL0.print("PM 10.0 (ug/m3): ");
      SERIAL0.println(data.PM_AE_UG_10_0);
      _pm2_5 = _pm2_5_bucket.add((float)data.PM_AE_UG_2_5);
      _pm10 = _pm10_bucket.add((float)data.PM_AE_UG_10_0);
      // SERIAL0.println();
    }
    else {
      SERIAL0.println("Invalid PM2.5 Sensor.");
      // _pm2_5 = _pm2_5_bucket.add(0.0);
      // _pm10 = _pm10_bucket.add(0.0);
      _invalid_count++;
      if (_invalid_count % 100000 == 0) {
        // SERIAL0.print(".");
        // SERIAL0.printf("invalid count =%d\r\n", _invalid_count);
      }

    //   if (_invalid_count/1000 > 500) {
    //     ESP.deepSleep(10e5);
    //   }
    }
  // });
}
