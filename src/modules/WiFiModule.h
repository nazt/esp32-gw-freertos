#include <CMMC_Module.h>

#ifndef CMMC_WIFI_MODULE_H
#define CMMC_WIFI_MODULE_H

// #DEFINE
class WiFiModule: public CMMC_Module {
  public:
    void config(CMMC_System *os, AsyncWebServer* server);
    void configSetup();
    void setup();
    void loop();
    void isLongPressed();
    void configLoop();
    const char* name() {
      return "WiFiModule";
    }
  protected:
    void configWebServer();
  private:
    char sta_ssid[30] = "";
    char sta_pwd[30] = "";
    char ap_pwd[30] = "";
    void _init_sta();
    bool _wifi_connected = false;
};

#endif
