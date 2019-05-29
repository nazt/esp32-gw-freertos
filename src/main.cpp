#include <Arduino.h>
#include <CMMC_Legend.h>
#include <WiFi.h>
#include <rom/rtc.h>

#include "modules/ConfigModule.h"
#include "modules/CMMC_LCD.h"
#include <HardwareSerial.h>
// #include <CMMC_DustSensor.h>
// #include <CMMC_RTC.h>

CMMC_Legend os;


void hook_init_ap(char* name, IPAddress ip) {
  Serial.println("----------- hook_init_ap -----------");
  Serial.println(name);
  Serial.println(ip);
  Serial.println("/----------- hook_init_ap -----------");
}

os_config_t config = {
  .baudrate = 115200,
  .BLINKER_PIN = 21,
  .BUTTON_MODE_PIN = 0,
  .SWITCH_PIN_MODE = INPUT_PULLUP,
  .SWITCH_PRESSED_LOGIC = LOW,
  .delay_after_init_ms = 200,
  .hook_init_ap = hook_init_ap
};

#include "tasks/app.hpp"

void setup()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);

  os.addModule(new CMMC_LCD());
  os.addModule(new ConfigModule());
  os.setup(&config);
  Serial.printf("free heap = %lu\r\n", ESP.getFreeHeap());
  Serial.printf("free heap = %lu\r\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));


  tasks_init();
  // dustSensor = new CMMC_DustSensor(&Serial1);
  // modules[0] = dustSensor;
  // modules[1] = new CMMC_GPS(&Serial1);
}

void loop()
{
  os.run();
}
