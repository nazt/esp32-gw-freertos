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
  // byte error, address;
  // int nDevices;
  //
  // SERIAL0.println("Scanning...");
  // Wire.begin();
  //
  // nDevices = 0;
  // for(address = 1; address < 127; address++ )
  // {
  //   // The i2c_scanner uses the return value of
  //   // the Write.endTransmisstion to see if
  //   // a device did acknowledge to the address.
  //   Wire.beginTransmission(address);
  //   error = Wire.endTransmission();
  //
  //   if (error == 0)
  //   {
  //     SERIAL0.print("I2C device found at address 0x");
  //     if (address<16)
  //       SERIAL0.print("0");
  //     SERIAL0.print(address,HEX);
  //     SERIAL0.println("  !");
  //
  //     nDevices++;
  //   }
  //   else if (error==4)
  //   {
  //     SERIAL0.print("Unknown error at address 0x");
  //     if (address<16)
  //       SERIAL0.print("0");
  //     SERIAL0.println(address,HEX);
  //   }
  // }
  // if (nDevices == 0)
  //   SERIAL0.println("No I2C devices found\n");
  // else
  //   SERIAL0.println("done\n");

  ina219->begin();
  ina219->setCalibration_16V_400mA();

  shuntvoltage = ina219->getShuntVoltage_mV();
  busvoltage = ina219->getBusVoltage_V();
  current_mA = ina219->getCurrent_mA();
  power_mW = ina219->getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
}

void PowerModule::loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  shuntvoltage = ina219->getShuntVoltage_mV();
  busvoltage = ina219->getBusVoltage_V();
  current_mA = ina219->getCurrent_mA();
  power_mW = ina219->getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
}
