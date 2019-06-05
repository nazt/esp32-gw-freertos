#include <CMMC_Module.h>

#ifndef CMMC_CONFIG_MODULE_H
#define CMMC_CONFIG_MODULE_H

class ConfigModule: public CMMC_Module {
  public:
    void config(CMMC_System *os, AsyncWebServer* server);
    void configSetup();
    void setup();
    void loop();
    void isLongPressed();
    void configLoop();
    const char* name() {
      return "ConfigModule";
    }
    char magel_token[80];
    char device_name[40];
  protected:
    void configWebServer();
  private:
};

#endif
