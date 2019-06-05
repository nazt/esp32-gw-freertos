#include "PowerModule.h"

// extern LCDModule* lcdModule;
extern HardwareSerial SERIAL0;

PowerModule::PowerModule() {
    ina219 = new Adafruit_INA219(0x40);
}

void PowerModule::isLongPressed() {

}

void PowerModule::configLoop() {
}

void PowerModule::configSetup() {
  // lcdModule->displayConfigWiFi();
}

void PowerModule::config(CMMC_System *os, AsyncWebServer *server)
{
  // curl -d "sta_ssid=ampere3&sta_password=espertap2" -X POST http://192.168.4.1/api/wifi/sta
  strcpy(this->path, "/api/config/power");
  static PowerModule *that = this;
  this->_serverPtr = server;
  this->_managerPtr = new CMMC_ConfigManager("/power.json");
  this->_managerPtr->init();
  this->_managerPtr->load_config([&](JsonObject *root, const char *content) {
    if (root == NULL) {
      SERIAL0.print("power.json failed. >");
      SERIAL0.println(content);
      return;
    }
    else {
      SERIAL0.println("[user] config json loaded..");
    }
  });
  this->configWebServer();
}

void PowerModule::configWebServer()
{
  static PowerModule *that = this;
  _serverPtr->on(this->path, HTTP_POST, [&](AsyncWebServerRequest *request) {
    String output = that->saveConfig(request, this->_managerPtr);
    request->send(200, "application/json", output);
  });
}

void PowerModule::setup()
{
  ina219->begin();
  ina219->setCalibration_16V_400mA();

  shuntvoltage = ina219->getShuntVoltage_mV();
  busvoltage = ina219->getBusVoltage_V();
  current_mA = ina219->getCurrent_mA();
  power_mW = ina219->getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);   
}

void PowerModule::loop() {
  shuntvoltage = ina219->getShuntVoltage_mV();
  busvoltage = ina219->getBusVoltage_V();
  current_mA = ina219->getCurrent_mA();
  power_mW = ina219->getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  SERIAL0.print("Bus Voltage:   "); SERIAL0.print(busvoltage); SERIAL0.println(" V");
  SERIAL0.print("Shunt Voltage: "); SERIAL0.print(shuntvoltage); SERIAL0.println(" mV");
  SERIAL0.print("Load Voltage:  "); SERIAL0.print(loadvoltage); SERIAL0.println(" V");
  SERIAL0.print("Current:       "); SERIAL0.print(current_mA); SERIAL0.println(" mA");
  SERIAL0.print("Power:         "); SERIAL0.print(power_mW); SERIAL0.println(" mW");
  SERIAL0.println("");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
