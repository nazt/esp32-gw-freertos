#ifndef CMMC_LEGEND_H
#define CMMC_LEGEND_H
#define CMMC_USE_ALIAS

#include <Arduino.h>
#include "version.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include "SPIFFS.h"

#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#include <FS.h>
#include <xCMMC_LED.h>
#include <CMMC_ConfigManager.h>
#include "CMMC_System.hpp"
#include <vector>
#include "CMMC_Module.h"


static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
static AsyncEventSource events("/events");
static xCMMC_LED *blinker;

static const char* http_username = "admin";
static const char* http_password = "admin";

enum MODE {SETUP, RUN};

typedef struct
{
    uint32_t baudrate;
    uint8_t BLINKER_PIN;
    uint8_t BUTTON_MODE_PIN;
    uint8_t SWITCH_PIN_MODE;
    bool SWITCH_PRESSED_LOGIC;
    uint32_t delay_after_init_ms;
    std::function<void(char*, IPAddress&)> hook_init_ap;
} os_config_t;


class CMMC_Legend: public CMMC_System {
  public:
    void addModule(CMMC_Module* module);
    void run();
    void isLongPressed();
    void setup(os_config_t *);

  xCMMC_LED *getBlinker();
  protected:
    void init_gpio();
    void init_fs();
    void init_user_sensor();
    void init_user_config();
    void init_network();
    bool setEnable(bool status);

  private:
    MODE mode;
    std::vector<CMMC_Module*> _modules;
    char ap_ssid[30] = "CMMC-Legend";
    void _init_ap();
    void setupWebServer(AsyncWebServer *server, AsyncWebSocket *ws, AsyncEventSource *events);
    bool stopFlag = false;
    uint8_t BLINKER_PIN;
    uint8_t button_gpio;
    bool SWITCH_PRESSED_LOGIC;
    uint8_t SWITCH_PIN_MODE;
    std::function<void(char*, IPAddress&)> _hook_init_ap;
};

#endif
