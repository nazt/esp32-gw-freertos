#include "ConfigModule.h"

// extern LCDModule* lcdModule;
extern HardwareSerial SERIAL0;
extern char magel_token[40];


void ConfigModule::isLongPressed() {
}

void ConfigModule::configLoop() {
}

void ConfigModule::configSetup() {
  // lcdModule->displayConfigWiFi();
}

void ConfigModule::config(CMMC_System *os, AsyncWebServer *server)
{
  // curl -d "sta_ssid=ampere3&sta_password=espertap2" -X POST http://192.168.4.1/api/wifi/sta

  strcpy(this->path, "/api/config/all");
  static ConfigModule *that = this;
  this->_serverPtr = server;
  this->_managerPtr = new CMMC_ConfigManager("/nat.json");
  this->_managerPtr->init();
  this->_managerPtr->load_config([&](JsonObject *root, const char *content) {
    if (root == NULL) {
      SERIAL0.print("config.json failed. >");
      SERIAL0.println(content);
      return;
    }
    else {
      SERIAL0.println("[user] config json loaded..");
      if ((*root)["maggel_id"] != NULL) {
        const char* maggel_id = (*root)["maggel_id"];
        SERIAL0.printf("MAGGEL_ID LOADED = %s\r\n", maggel_id);
        strcpy(magel_token, maggel_id);
      }
      else {
        SERIAL0.println("MAGGEL_ID is NULL.");
        strcpy(magel_token, "FFFF-FFFF-FFFF");
      }
    }
    // const char *sta_config[2];
    // if ((sta_config[0] == NULL) || (sta_config[1] == NULL))
    // {
    //   SERIAL0.println("NULL..");
    //   SPIFFS.remove("/enabled");
    //   return;
    // };
    // strcpy(that->sta_ssid, sta_config[0]);
    // strcpy(that->sta_pwd, sta_config[1]);
  });
  this->configWebServer();
}

void ConfigModule::configWebServer()
{
  static ConfigModule *that = this;
  _serverPtr->on(this->path, HTTP_POST, [&](AsyncWebServerRequest *request) {
    String output = that->saveConfig(request, this->_managerPtr);
    request->send(200, "application/json", output);
  });
}

void ConfigModule::setup()
{
  // _init_sta();
}

void ConfigModule::loop() {

}
