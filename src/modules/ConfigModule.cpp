#include "ConfigModule.h"

// extern LCDModule* lcdModule;
extern HardwareSerial SERIAL0;


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

  strcpy(this->path, "/api/config/nbiot");
  static ConfigModule *that = this;
  this->_serverPtr = server;
  this->_managerPtr = new CMMC_ConfigManager("/nbiot.json");
  this->_managerPtr->init();
  this->_managerPtr->load_config([&](JsonObject *root, const char *content) {
    if (root == NULL) {
      SERIAL0.print("config.json failed. >");
      SERIAL0.println(content);
      return;
    }
    else {
      SERIAL0.println("[user] config json loaded..");
      root->printTo(SERIAL0);
      SERIAL0.println();
      if (root->get<const char*>("magel_id") != NULL) {
        const char* magel_id = root->get<const char*>("magel_id");
        SERIAL0.printf("magel_ID LOADED = %s\r\n", magel_id);
        strcpy(that->magel_token, magel_id);
      }
      else {
        SERIAL0.println("magel_ID is NULL.");
        strcpy(that->magel_token, "FFFF-FFFF-FFFF");
      }

      if (root->get<const char*>("device_name") != NULL) {
        const char* device_name = root->get<const char*>("device_name");
        SERIAL0.printf("device_name LOADED = %s\r\n", device_name);
        strcpy(that->device_name, device_name);
      }
      else {
        strcpy(that->device_name, "NONAME");
        SERIAL0.println("device_name is NULL.");
      }
    }
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
