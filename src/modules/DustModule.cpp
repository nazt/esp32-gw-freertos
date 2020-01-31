#include "DustModule.h"
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
      Serial.println("---------------");
      Serial.print("pm2.5> ");
      Serial.println(_pm2_5);
      Serial.print("pm10> ");
      Serial.println(_pm2_5);
      Serial.println("---------------");

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
  // ti.every_ms(1, [&]() {
    if (pms->read(data)) {
      _invalid_count = 0;
      // // Serial.println();
      // // Serial.print("PM 1.0 (ug/m3): ");
      // // Serial.println(data.PM_AE_UG_1_0);
      // //
      // // Serial.print("PM 2.5 (ug/m3): ");
      // Serial.println(data.PM_AE_UG_2_5);
      _pm2_5 = _pm2_5_bucket.add((float)data.PM_AE_UG_2_5);
      // Serial.print("PM 10.0 (ug/m3): ");
      // Serial.println(data.PM_AE_UG_10_0);
      _pm10 = _pm10_bucket.add((float)data.PM_AE_UG_10_0);
      // Serial.println();
    }
    else {
      // Serial.println("Invalid PM2.5 Sensor.");
      // _pm2_5 = _pm2_5_bucket.add(0.0);
      // _pm10 = _pm10_bucket.add(0.0);
      _invalid_count++;
      if (_invalid_count % 100000 == 0) {
        // Serial.print(".");
        // Serial.printf("invalid count =%d\r\n", _invalid_count);
      }

    //   if (_invalid_count/1000 > 500) {
    //     ESP.deepSleep(10e5);
    //   }
    }
  // });
}
