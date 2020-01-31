  #include "tasks/dust/CMMC_DustSensor.h"
#include "tasks/lcd/CMMC_LCD.h"
#include <TinyGPS++.h>

#include "coap.h"
#include "coap-helper.h"

static const int RX_BUF_SIZE = 1024;

extern char sta_mac[18];
extern char softap_mac[18];
extern float G_busvoltage;
extern unsigned int G_sent;
extern int G_modem_type;
extern char G_device_name[40];


typedef enum {
  TYPE_KEEP_ALIVE = 1,
  TYPE_SENSOR_NODE
} DATA_COAP_TYPE;

typedef struct{
  float  batt_volt = 0;
  float pm10;
  float pm2_5;
  uint32_t uptime_s;
  uint32_t unixtime;
  uint32_t rebootCount = 0;
  char latlngC[80];
  unsigned int ct = 1;
  DATA_COAP_TYPE packet_type;
  int modem_type;
} Data;

struct shared_pool {
  float pm10;
  float pm2_5;
  String locationString;
};

String nb_status_string = "...";
static uint32_t ct = 0;

static struct shared_pool pool;
static CMMC_LCD *lcd = NULL;

static struct shared_pool p;
#include "modem/CMMC_Modem.h"
CMMC_Modem *modem;
// extern static QueueHandle_t xQueueMain;

// void showDate(const char* txt, const DateTime& dt) {
//   SERIAL0.print(txt);
//   SERIAL0.printf("%d/%d/%d %02d:%02d:%02d", dt.day(), dt.month(),
//     dt.year(), dt.hour(), dt.minute()%60, dt.second()%60);
// }

// #define RTC_MODULE 1
// #define GPS_MODULE 1

static shared_pool p2 = pool;
static void task_serial1(void *parameter) {
  // SERIAL0.println("Initializing task_serial1.");
  // pool.pm10 = 0;
  // pool.pm2_5 = 0;

  // SERIAL0.println("Initializing dust.");
  static CMMC_DustSensor *dustSensor = new CMMC_DustSensor(&Serial1);
  dustSensor->setup();

  // #ifdef GPS_MODULE
  // SERIAL0.println("Initializing gps.");
  // static CMMC_GPS *gps = new CMMC_GPS(&Serial1);
  // gps->setup();
  // #endif
  //
  // #ifdef RTC_MODULE
  // SERIAL0.println("Initializing rtc.");
  // static CMMC_RTC *rtc = new CMMC_RTC();
  // rtc->setup();
  // #endif


  while (1) {
    vTaskDelay(200 / portTICK_PERIOD_MS);
    if (lcd != NULL) {
    }
    if (xpage == LCD_CONFIG) {
      continue;
    }
    dustSensor->loop();

    pool.pm10 = dustSensor->getPMValue(DustPM10);
    pool.pm2_5 = dustSensor->getPMValue(DustPM2_5);

    if (xQueueMain != NULL) {
        const TickType_t xTicksToWait = pdMS_TO_TICKS(1000);
        BaseType_t xStatus = xQueueSendToBack(xQueueMain, &p2, xTicksToWait);
        if ( xStatus == pdPASS ) {
          // if (!modem->isLocked())
            // SERIAL0.printf("[ENQUEU!!] queue size = %lu \r\n", uxQueueMessagesWaiting(xQueueMain));
        }
        else {
          // if (!modem->isLocked())
          //   SERIAL0.println("FAIL TO ENQUEUE.");
        }
      }
      vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}

static void lcd_task(void *parameter) {
  lcd = new CMMC_LCD();
  lcd->setup();
  while (1) {
    SERIAL0.print(".");
    // SERIAL0.printf(">>%lu\n", millis( ));
    lcd->pm10 = pool.pm10;
    lcd->pm2_5 = pool.pm2_5;
    lcd->loop();
    // SERIAL0.printf("//%lu\n", millis());
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}



static char jsonBuffer[1024];
static uint8_t _buffer[2000];

static void nb_task(void *parameter) {
    SERIAL0.println("NB_TASK..");
    HardwareSerial NBSerial(2);
    NBSerial.begin(9600, SERIAL_8N1, 26 /*rx*/, 27 /* tx */);
    NBSerial.setTimeout(4);
    modem = new CMMC_Modem(&NBSerial, &SERIAL0, -1);
    modem->setup();
    while (1) {
      // SERIAL0.println("NB_STASKING...");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      modem->loop();
      nb_status_string = modem->status;
      SERIAL0.printf("status=%s\r\n", nb_status_string.c_str());
      if (modem->isLocked()) {
        SERIAL0.println("NB-IoT LOCKED.");
        continue;
      }
      if (xQueueMain != NULL) {
        const TickType_t xTicksToWait = pdMS_TO_TICKS(10);
        BaseType_t xStatus;
        xStatus = xQueueReceive(xQueueMain, &p, xTicksToWait);
        if (xStatus == pdPASS) {
          SERIAL0.println("[X-TASK] QUEUE RECV...");
          SERIAL0.println(p.pm10);
          SERIAL0.println(p.pm2_5);
          Data data;
          data.pm10 = pool.pm10;
          data.pm2_5 = pool.pm2_5;
          data.ct = ct++;
          data.modem_type = G_modem_type;
          data.uptime_s = millis() / 1000;
          // data.unixtime = pool.dt.unixtime();
          data.batt_volt = G_busvoltage;
          int freshGps = -1;
          strcpy(data.latlngC, pool.locationString.c_str());
          if(pool.locationString[0] == '0') {
            freshGps = 0;
          }
          else {
            freshGps = 1;
          }
          // if (data.packet_type == TYPE_KEEP_ALIVE) {
          sprintf(jsonBuffer, "{\"ap\": \"%s\", \"pm10\":%s,\"pm2_5\":%s,\"boot_count\":%d,\"uptime_s\":%lu,\"heap\":%lu,\"ct\":%lu, \"nickname\": \"%s\"}", softap_mac, String(data.pm10).c_str(), String(data.pm2_5).c_str(),  data.rebootCount, data.uptime_s,  ESP.getFreeHeap(), data.ct++, G_device_name);
          SERIAL0.println(jsonBuffer);

          // Serial.printf("jsonBuffer= %s\r\n", jsonBuffer);
          // // Serial.printf("   buffer = %s\r\n", _buffer);
          // // DUSTBOY2_1
          // uint16_t buflen = generate(_buffer, aisip, 5683, ("NBIoT/" DUSTBOY_ID),
          // COAP_CON, COAP_POST, NULL, 0, (uint8_t*) jsonBuffer, strlen(jsonBuffer));
          // Serial.printf("      len = %d\r\n", buflen);
          // that->sendPacket((uint8_t*)_buffer, buflen);
          // 6229a600-62da-11e9-96dd-9fb5d8a71344

          static char path[100];
          strcpy(path, "NBIoT/");
          strcpy(path+6, G_magel_token);
          SERIAL0.println(path);
          IPAddress aisip = IPAddress(103, 20, 205, 85);
          uint16_t buflen = generate(_buffer, aisip, 5683, path,
          COAP_CON, COAP_POST, NULL, 0, (uint8_t*) jsonBuffer, strlen(jsonBuffer));
          SERIAL0.printf("      len = %d\r\n", buflen);
          nb_status_string = "dispatching...";
          modem->sendPacket((uint8_t*)_buffer, buflen);
          nb_status_string = "sent.";
          G_sent++;
        }
        else {
          // SERIAL0.println("FAILED TO RECV Q.");
        }
      }
    }
    SERIAL0.println("Ending task 2");
    vTaskDelete( NULL );
}

static void tasks_init() {
  int priority = 1;
  xTaskCreate(task_serial1, "task_serial1", 8192, NULL, priority, NULL);
  delay(1000);
  xTaskCreate(nb_task, "nb_task", 8192, NULL, priority, NULL);
  // xTaskCreatePinnedToCore(task_serial1, "task_serial1", 2048, NULL, priority, NULL, 1);
}
