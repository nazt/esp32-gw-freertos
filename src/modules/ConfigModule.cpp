#include "ConfigModule.h"

#define WIFI_CONFIG_FILE "/wifi.json"

// extern LCDModule* lcdModule;

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
  strcpy(this->path, "/api/wifi/sta");
  static ConfigModule *that = this;
  this->_serverPtr = server;
  this->_managerPtr = new CMMC_ConfigManager(WIFI_CONFIG_FILE);
  this->_managerPtr->init();
  this->_managerPtr->load_config([](JsonObject *root, const char *content) {
    if (root == NULL) {
      Serial.print("config.json failed. >");
      Serial.println(content);
      return;
    }
    Serial.println("[user] wifi config json loaded..");
    const char *sta_config[2];
    sta_config[0] = (*root)["sta_ssid"];
    sta_config[1] = (*root)["sta_password"];
    // if ((sta_config[0] == NULL) || (sta_config[1] == NULL))
    // {
    //   Serial.println("NULL..");
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
