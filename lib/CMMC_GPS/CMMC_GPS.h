#ifndef CMMC_GPS_H
#define CMMC_GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include <CMMC_Module.h>

class CMMC_GPS: public CMMC_Module{
  Stream *serial;
  TinyGPSPlus gps;
  char latC[20];
  char lngC[20];
  char latlngC[40];
  bool gpsNoSignal = 1;
  protected:
    // char path[20];
    // String saveConfig(AsyncWebServerRequest *request, CMMC_ConfigManager* configManager);
  public:
    CMMC_GPS(Stream*);
    // virtual void config(CMMC_System *os, AsyncWebServer* server) = 0;
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    // void configWebServer();
};

#endif
