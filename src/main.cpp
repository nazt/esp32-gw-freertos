#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "defs.h"
#include <HardwareSerial.h>
#include <WiFi.h>
#include <rom/rtc.h>
#include <CMMC_Legend.h>


#include "modules/ConfigModule.h"
#include "tasks/lcd/CMMC_LCD.h"
#include "utils.hpp"

SCREEN_PAGE xpage = LCD_LOGO;
char ap_name[20];

char G_magel_token[80];
char G_device_name[20];

char sta_mac[18];
char softap_mac[18];
CMMC_Legend *os;
HardwareSerial SERIAL0(0);
ConfigModule* configModule;

void hook_init_ap(char* name, IPAddress ip) {
  strcpy(ap_name, name);
  SERIAL0.println("----------- hook_init_ap -----------");
  SERIAL0.println(name);
  SERIAL0.println(ip);
  SERIAL0.println("/----------- hook_init_ap -----------");
  xpage = LCD_CONFIG;
}

void hook_button_pressed() {
  SERIAL0.println("[user] hook_button_pressed");
  // xpage = LCD_BUTTON_PRESSED;
}

void hook_button_released() {
  SERIAL0.println("[user] hook_button_released");
  if (xpage == LCD_RUN) {
    xpage = LCD_DETAIL_PACKET;
  }
  else if (xpage == LCD_DETAIL_PACKET){
    xpage = LCD_RUN;
  }
  else {
    xpage = LCD_RUN;
  }
}

void hook_button_long_pressed() {
  SERIAL0.println("[user] hook_button_long_pressed");
  xpage = LCD_BUTTON_LONG_PRESSED;
}

void hook_ready();

QueueHandle_t xQueueMain;

#include "tasks/app.hpp"

void setup()
{
  WiFi.mode(WIFI_AP_STA);
  strcpy(sta_mac, WiFi.macAddress().c_str());
  strcpy(softap_mac, WiFi.softAPmacAddress().c_str());

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  SERIAL0.begin(115200);

  SERIAL0.printf("STA MAC: %s\r\n", sta_mac);
  SERIAL0.printf(" AP MAC: %s\r\n", softap_mac);

  xQueueMain = xQueueCreate(30, sizeof(struct shared_pool));

  String taskMessage = "[main] Task running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  SERIAL0.println(taskMessage);

  if(xQueueMain == NULL){
    SERIAL0.println("Error creating the queue");
  }
  else {
    SERIAL0.println("creating queue done.");
  }
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
    .hook_ready = hook_ready,
  };

  configModule = new ConfigModule();
  os->addModule(configModule);
  tasks_init();
  os->setup(&config);

}

void hook_ready() {
  strcpy(G_magel_token, configModule->magel_token);
  strcpy(G_device_name, configModule->device_name);
  xpage = LCD_RUN;
  SERIAL0.printf("free heap = %lu\r\n", ESP.getFreeHeap());
  SERIAL0.printf("free heap = %lu\r\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}

uint32_t prev = 0;

void loop()
{
  os->run();
      // if (xQueueMain != NULL) {
      //   BaseType_t xStatus;
      //   const TickType_t xTicksToWait = pdMS_TO_TICKS(10);
      //   struct shared_pool p;
      //   xStatus = xQueueReceive(xQueueMain, &p, xTicksToWait);
      //   /* check whether receiving is ok or not */
      //   if(xStatus == pdPASS){
      //     SERIAL0.println("[X-TASK] QUEUE RECV...");
      //   }
      //   // vTaskDelay(500/portTICK_PERIOD_MS); //wait for 500 ms
      // }

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
