#ifndef CMMC_INA219_H
#define CMMC_INA219_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <Adafruit_INA219.h>


class CMMC_INA219: public CMMC_Module{
  float shuntvoltage, busvoltage, loadvoltage;
  double current_mA, current_A, sumCurrent;
  float power_mW = 0, power_W = 0;
  uint32_t pevTime = 0, pevPrint = 0;
  uint32_t powerCount = 0;
  double powerSum = 0;
  double powerAverage = 0;
  double powerHour = 0, powerWattHour, powerKilosWattHour;
  Adafruit_INA219 ina219(0x41);
  protected:
    // char path[20];
    // String saveConfig(AsyncWebServerRequest *request, CMMC_ConfigManager* configManager);
  public:
    CMMC_INA219();
    // virtual void config(CMMC_System *os, AsyncWebServer* server) = 0;
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    // void configWebServer();
};

#endif
