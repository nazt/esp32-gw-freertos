#include <Arduino.h>
#include "defs.h"
#include <HardwareSerial.h>
#include <WiFi.h>
#include <rom/rtc.h>
#include <CMMC_Legend.h>


#include "modules/ConfigModule.h"
#include "tasks/lcd/CMMC_LCD.h"
#include "utils.hpp"

SCREEN_PAGE xpage = LCD_RUN;
char ap_name[20];

CMMC_Legend *os;
HardwareSerial SERIAL0(0);

void hook_init_ap(char* name, IPAddress ip) {
  strcpy(ap_name, name);
  SERIAL0.println("----------- hook_init_ap -----------");
  SERIAL0.println(name);
  SERIAL0.println(ip);
  SERIAL0.println("/----------- hook_init_ap -----------");
  xpage = LCD_CONFIG;
  // lcd->setApName(ap_name);
}

void hook_button_pressed() {
  SERIAL0.println("[user] hook_button_pressed");
  xpage = LCD_BUTTON_PRESSED;
}

void hook_button_released() {
  SERIAL0.println("[user] hook_button_released");
  xpage = LCD_RUN;
}

void hook_button_long_pressed() {
  SERIAL0.println("[user] hook_button_long_pressed");
  xpage = LCD_BUTTON_LONG_PRESSED;
}


#include "tasks/app.hpp"

void setup()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  SERIAL0.begin(115200);
  String taskMessage = "[main] Task running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  SERIAL0.println(taskMessage);
  // delay(200);
  SERIAL0.println("CPU0 reset reason:");
  print_reset_reason(rtc_get_reset_reason(0), &SERIAL0);
  verbose_print_reset_reason(rtc_get_reset_reason(0), &SERIAL0);

  SERIAL0.println("CPU1 reset reason:");
  print_reset_reason(rtc_get_reset_reason(1), &SERIAL0);
  verbose_print_reset_reason(rtc_get_reset_reason(1), &SERIAL0);

  os = new CMMC_Legend(&SERIAL0);

  static os_config_t config = {
    .BLINKER_PIN = 21,
    .BUTTON_MODE_PIN = 0,
    .SWITCH_PIN_MODE = INPUT_PULLUP,
    .SWITCH_PRESSED_LOGIC = LOW,
    .delay_after_init_ms = 200,
    .hook_init_ap = hook_init_ap,
    .hook_button_pressed =  hook_button_pressed,
    .hook_button_long_pressed = hook_button_long_pressed,
    .hook_button_released = hook_button_released,
  };

  // os->addModule(new ConfigModule());

  tasks_init();

  os->setup(&config);
  //
  SERIAL0.printf("free heap = %lu\r\n", ESP.getFreeHeap());
  SERIAL0.printf("free heap = %lu\r\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}

uint32_t prev = 0;

void loop()
{
  os->run();
  // String taskMessage = "[main] Task running on core ";
  // taskMessage = taskMessage + xPortGetCoreID();
  // lcd->pm2_5 = pool.pm2_5;
  // lcd->pm10 = pool.pm10;
  // // SERIAL0.println(taskMessage);
  if ( (millis() - prev) > 1*1000L) {
    // SERIAL0.println(ESP.getFreeHeap());
    prev = millis();
  //   // lcd->pm10 = millis()/1000;
  //   lcd->loop();
  }
}
