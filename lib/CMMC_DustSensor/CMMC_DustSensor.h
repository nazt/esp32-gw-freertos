#ifndef CMMC_DustSensor_H
#define CMMC_DustSensor_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <CMMC_NB_IoT.h>
#define MAX_ARRAY 120
class CMMC_DustSensor: public CMMC_Module{
  protected:
    CMMC_NB_IoT *nb;
    Stream *_serial;
    int isNbConnected = 1;
    float pm25; //เก็บฝุ่น pm2.5
    float pm10; //เก็บฝุ่น pm10
    float dust_average25; //เก็บค่าเฉลี่ยฝุ่น pm2.5
    float dust_average10; //เก็บค่าเฉลี่ยฝุ่น pm10
    float temperature_c,  humidity_percent_rh = 0;
    int dustIdx = 0;
    float pm25_array[MAX_ARRAY] = { 0.0 };
    float pm10_array[MAX_ARRAY] = { 0.0 };
  public:
    CMMC_DustSensor(Stream*);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
    void readDustSensor();
};

#endif
