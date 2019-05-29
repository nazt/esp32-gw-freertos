#include "tasks/dust/CMMC_DustSensor.h"
#include "tasks/gps/CMMC_GPS.h"
static const int RX_BUF_SIZE = 1024;
static void task_serial1(void * parameter) {
    static CMMC_DustSensor *dustSensor = new CMMC_DustSensor(&Serial1);
    static CMMC_GPS *gps = new CMMC_GPS(&Serial1);
    dustSensor->setup();
    gps->setup();
    while(1) {
      dustSensor->loop();
      gps->loop();
    }
}


void tasks_init() {
  int priority = 1;
  xTaskCreate(task_serial1, "task_serial1", 2048, NULL, priority, NULL);
}
