#ifndef CMMC_LEGEND_H
#define CMMC_LEGEND_H

#include <Arduino.h>
#include "version.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <HardwareSerial.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
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

enum MODE {CONFIG, RUN};

typedef struct
{
    uint8_t BLINKER_PIN;
    uint8_t BUTTON_MODE_PIN;
    uint8_t SWITCH_PIN_MODE;
    bool SWITCH_PRESSED_LOGIC;
    uint32_t delay_after_init_ms;
    std::function<void(char*, IPAddress&)> hook_init_ap;

    std::function<void(void)> hook_button_pressed;
    std::function<void(void)> hook_button_long_pressed;
    std::function<void(void)> hook_button_released;
    std::function<void(void)> hook_ready;
    std::function<void(void)> hook_config_loaded;
} os_config_t;


class CMMC_Legend: public CMMC_System {
  public:
    CMMC_Legend(HardwareSerial *);
    void addModule(CMMC_Module* module);
    void isLongPressed();
    void run();
    void setup(os_config_t *);
    void configSetup(os_config_t *);
    void runSetup(os_config_t *);

  xCMMC_LED *getBlinker();
  protected:
    void init_gpio();
    void init_fs();
    void init_user_sensor();
    void init_user_config();
    void init_network();
    bool enable_run_mode(bool status);

  private:
    MODE mode;
    std::vector<CMMC_Module*> _modules;
    void _init_ap();
    void setupWebServer(AsyncWebServer *server, AsyncWebSocket *ws, AsyncEventSource *events);
    char ap_ssid[30] = "CMMC-Legend";
    bool stopFlag = false;
    uint8_t BLINKER_PIN;
    uint8_t button_gpio;
    bool SWITCH_PRESSED_LOGIC;
    uint8_t SWITCH_PIN_MODE;
    std::function<void(char*, IPAddress&)> _hook_init_ap;
    std::function<void(void)> _hook_button_pressed;
    std::function<void(void)> _hook_button_long_pressed;
    std::function<void(void)> _hook_button_released;
    std::function<void(void)> _hook_ready;
    std::function<void(void)> _hook_config_loaded;
    HardwareSerial *_serial = NULL;
    HardwareSerial *_serial_legend = NULL;
};

#endif
