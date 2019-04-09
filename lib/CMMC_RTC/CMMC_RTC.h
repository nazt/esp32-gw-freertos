#ifndef CMMC_RTC_H
#define CMMC_RTC_H

#include <Arduino.h>
#include <CMMC_Module.h>

class CMMC_RTC: public CMMC_Module{
  protected:
  public:
    CMMC_RTC();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
    void displayLogo();
};

#endif
