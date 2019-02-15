#ifndef CMMC_MODULE_H
#define CMMC_MODULE_H

#include <Arduino.h>

class CMMC_Module {
  protected:
    // CMMC_ConfigManager *_managerPtr;
    // AsyncWebServer *_serverPtr;
    // char path[20];
    // String saveConfig(AsyncWebServerRequest *request, CMMC_ConfigManager* configManager);
  public:
    // virtual void config(CMMC_System *os, AsyncWebServer* server) = 0;
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void configLoop();
    virtual void configSetup();
    // void configWebServer();
};

#endif
