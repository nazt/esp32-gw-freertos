#ifndef CMMC_GPS_H
#define CMMC_GPS_H

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <TinyGPS++.h>
#include <CMMC_Module.h>
#include <HardwareSerial.h>

static char _bufferLatLng[100];
class CMMC_GPS: public CMMC_Module{
  HardwareSerial *serial;
  char latC[40];
  char lngC[40];
  char latlngC[80];
  bool gpsNoSignal = 1;
  uint32_t _lastSyncRtc = 0;
  uint32_t _lastFetchLocation = 0;
  DateTime _dt;
  protected:
    // char path[20];
    // String saveConfig(AsyncWebServerRequest *request, CMMC_ConfigManager* configManager);
  public:
    TinyGPSPlus gps;
    CMMC_GPS(HardwareSerial*);
    void config(CMMC_System *os, AsyncWebServer* server) {};
    const char* name() {
      return "CMMC_GPS";
    }
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    String getLocation();
    DateTime getDateTime();
    // void configWebServer();
};

#endif
