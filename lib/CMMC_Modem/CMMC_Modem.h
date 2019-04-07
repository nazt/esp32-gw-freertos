#ifndef CMMC_Modem_H
#define CMMC_Modem_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <U8g2lib.h>


class CMMC_Modem: public CMMC_Module{
  protected:
  public:
    CMMC_Modem();
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
};

#endif
