#include "tasks/dust/CMMC_DustSensor.h"
#include "tasks/gps/CMMC_GPS.h"
#include "tasks/rtc/CMMC_RTC.h"
#include "tasks/lcd/CMMC_LCD.h"
#include <TinyGPS++.h>

#include "coap.h"
#include "coap-helper.h"

static const int RX_BUF_SIZE = 1024;

extern char sta_mac[18];
extern char softap_mac[18];
extern float G_busvoltage;
extern int G_modem_type;

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
  DateTime dt;
  String locationString;
};

String nb_status_string = "...";
static uint32_t ct = 0;

static struct shared_pool pool;
static CMMC_LCD *lcd = NULL;
// extern static QueueHandle_t xQueueMain;

void showDate(const char* txt, const DateTime& dt) {
  SERIAL0.print(txt);
  SERIAL0.printf("%d/%d/%d %02d:%02d:%02d", dt.day(), dt.month(),
    dt.year(), dt.hour(), dt.minute()%60, dt.second()%60);
}

static shared_pool p2 = pool;
static void task_serial1(void *parameter) {
  pool.pm10 = 0;
  pool.pm2_5 = 0;
  static CMMC_DustSensor *dustSensor = new CMMC_DustSensor(&Serial1);
  static CMMC_GPS *gps = new CMMC_GPS(&Serial1);
  static CMMC_RTC *rtc = new CMMC_RTC();
  dustSensor->setup();
  gps->setup();
  rtc->setup();
  while (1) {
    vTaskDelay(200 / portTICK_PERIOD_MS);
    if (lcd != NULL) {
      strcpy(lcd->dateTimeString, rtc->getDateTimeString().c_str());
    }
    if (xpage == LCD_CONFIG) {
      continue;
    }

    dustSensor->loop();
    gps->loop();
    rtc->loop();

    pool.pm10 = dustSensor->getPMValue(DustPM10);
    pool.pm2_5 = dustSensor->getPMValue(DustPM2_5);
    pool.dt = rtc->getDateTime();
    pool.locationString = gps->getLocationString();
    if (gps->_lastSyncRtc > 0) {
      rtc->adjust(gps->getDateTime());
    }
    else {
    }

    if (xQueueMain != NULL) {
        const TickType_t xTicksToWait = pdMS_TO_TICKS(1000);
        BaseType_t xStatus = xQueueSendToBack(xQueueMain, &p2, xTicksToWait);
        if ( xStatus == pdPASS ) {
          SERIAL0.printf("[ENQUEU!!] queue size = %lu \r\n", uxQueueMessagesWaiting(xQueueMain));
        }
        else {
          SERIAL0.println("FAIL TO ENQUEUE.");
        }
      }
      vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

static void lcd_task(void *parameter) {
  lcd = new CMMC_LCD();
  lcd->setup();
  while (1) {
    // SERIAL0.printf(">>%lu\n", millis());
    lcd->pm10 = pool.pm10;
    lcd->pm2_5 = pool.pm2_5;
    lcd->loop();
    // SERIAL0.printf("//%lu\n", millis());
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

static struct shared_pool p;
#include "modem/CMMC_Modem.h"
CMMC_Modem *modem;


static char jsonBuffer[1024];
static uint8_t _buffer[2000];

static void nb_task(void *parameter) {
    SERIAL0.println("NB_TASK..");
    HardwareSerial NBSerial(2);
    NBSerial.begin(9600, SERIAL_8N1, 26 /*rx*/, 27 /* tx */);
    NBSerial.setTimeout(4);
    modem = new CMMC_Modem(&NBSerial, &SERIAL0, G_modem_type);
    modem->setup();
    while (1) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      modem->loop();
      nb_status_string = modem->status;
      SERIAL0.printf("status=%s\r\n", nb_status_string);
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
          data.unixtime = pool.dt.unixtime();
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
          sprintf(jsonBuffer, "{\"ap\": \"%s\", \"pm10\":%s,\"pm2_5\":%s,\"loc\":\"%s\",\"reset\":%d,\"freshGps\":%d,\"uptime_s\":%lu,\"unixtime\":%lu,\"heap\":%lu,\"batt_volt\":%s,\"ct\":%lu,\"sleep\":%lu,\"payload\":\"%s\", \"modem_type\": %d}", softap_mac, String(data.pm10).c_str(), String(data.pm2_5).c_str(), data.latlngC, data.rebootCount, freshGps, data.uptime_s, data.unixtime,  ESP.getFreeHeap(), String(data.batt_volt).c_str(), data.ct++, 0, "X", modem->_modemType);
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
  int priority = 2;
  xTaskCreate(task_serial1, "task_serial1", 8192, NULL, priority, NULL);
  delay(1000);
  xTaskCreate(nb_task, "nb_task", 8192, NULL, 1, NULL);
  // xTaskCreatePinnedToCore(task_serial1, "task_serial1", 2048, NULL, priority, NULL, 1);
}
