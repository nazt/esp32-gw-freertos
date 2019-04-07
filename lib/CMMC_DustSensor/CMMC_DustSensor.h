#ifndef CMMC_DustSensor_H
#define CMMC_DustSensor_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <CMMC_NB_IoT.h>

class CMMC_DustSensor: public CMMC_Module{
  protected:
    CMMC_NB_IoT *nb;
    Stream *_modemSerial;
    int isNbConnected = 1;
  public:
    CMMC_DustSensor(Stream*);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
};

#endif
