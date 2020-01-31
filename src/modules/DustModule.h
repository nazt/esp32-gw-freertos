#include <CMMC_Module.h>

#ifndef CMMC_DUST_MODULE_H
#define CMMC_DUST_MODULE_H

#include <WiFi.h>
#include <CMMC_Interval.h>
#include <Arduino.h>
#include "PMS.h"
#include <CMMC_Bucket.h>


// #DEFINE
static    CMMC_Bucket _pm10_bucket;
static    CMMC_Bucket _pm2_5_bucket;
class DustModule: public CMMC_Module {
  public:
    DustModule(HardwareSerial*);
    void config(CMMC_System *os, AsyncWebServer* server);
    void configSetup();
    void setup();
    void loop();
    void isLongPressed();
    void configLoop();
    float getPm10();
    float getPm2_5();
    const char* name() {
      return "DustModule";
    }
    PMS::DATA data;
  protected:
    void configWebServer();
  private:
    CMMC_Interval ti;
    PMS *pms;
    float _pm10 = 0;
    float _pm2_5 = 0;
    uint32_t _invalid_count = 0;
};

#endif
