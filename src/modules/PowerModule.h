#include <CMMC_Module.h>
#include <Adafruit_INA219.h>

#ifndef CMMC_POWER_MODULE_H
#define CMMC_POWER_MODULE_H

class PowerModule: public CMMC_Module {
  public:
    PowerModule();
    void config(CMMC_System *os, AsyncWebServer* server);
    void configSetup();
    void setup();
    void loop();
    void isLongPressed();
    void configLoop();
    const char* name() {
      return "PowerModule";
    }

    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power_mW = 0;
    
  protected:
    void configWebServer();
  private:
    Adafruit_INA219 *ina219;

};

#endif
