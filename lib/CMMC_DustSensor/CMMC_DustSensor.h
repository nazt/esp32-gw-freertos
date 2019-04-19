#ifndef CMMC_DustSensor_H
#define CMMC_DustSensor_H

#include <Arduino.h>
#include <CMMC_Module.h>

#define MAX_ARRAY 120
typedef enum {
  DustPM10, DustPM2_5
}  dustType_t;


class CMMC_DustSensor: public CMMC_Module {
  protected:
    HardwareSerial *_serial;
    // float pm25; //เก็บฝุ่น pm2.5
    // float pm10; //เก็บฝุ่น pm10
    float dust_average25 = 0; //เก็บค่าเฉลี่ยฝุ่น pm2.5
    float dust_average10 = 0; //เก็บค่าเฉลี่ยฝุ่น pm10
    float temperature_c,  humidity_percent_rh = 0;
    int dustIdx = 0;
    float pm25_array[MAX_ARRAY] = { 0.0 };
    float pm10_array[MAX_ARRAY] = { 0.0 };
    unsigned int dust_counter = 0;
    void _calculateDustAverage();
  public:
    CMMC_DustSensor(HardwareSerial*);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void readDustSensor();
    float getPMValue(dustType_t);
};

#endif
