#include <Arduino.h>
#include <WiFi.h>
#include <rom/rtc.h>
#include <CMMC_GPS.h>
#include <CMMC_LCD.h>
#include <CMMC_NB_IoT.h>
#include <HardwareSerial.h>
#include <CMMC_Modem.h>
#include <CMMC_DustSensor.h>
#include <CMMC_RTC.h>

void verbose_print_reset_reason(RESET_REASON reason);
void print_reset_reason(RESET_REASON reason);
HardwareSerial NBSerial(2);
RTC_DATA_ATTR static int rebootCount = -1;

// struct queue_data {
//   int deviceId;
//   int measurementType;
//   float value;
// };
static char sta_mac[18];
static char softap_mac[18];
#define EXT_WDT_PIN 14

CMMC_LCD *lcd;
CMMC_GPS *gps;
CMMC_Modem *modem;
CMMC_RTC *rtc;
CMMC_DustSensor *dustSensor;

const int MODULE_SIZE = 2;
CMMC_Module* modules[10];
void nbTask( void * parameter);
void lcdTask( void * parameter);

// static QueueHandle_t xQueueMain;

void setup() {
  rebootCount++;
  // xQueueMain = xQueueCreate(1, sizeof(int));
  //
  // if(xQueueMain == NULL){
  //   Serial.println("Error creating the queue");
  // }
  Serial.begin(115200);
  Serial.print(">> CPU0 reset reason: ");
  print_reset_reason(rtc_get_reset_reason(0));
  verbose_print_reset_reason(rtc_get_reset_reason(0));
  Serial.print(">> CPU1 reset reason:");
  print_reset_reason(rtc_get_reset_reason(1));
  verbose_print_reset_reason(rtc_get_reset_reason(1));

  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  strcpy(sta_mac, WiFi.macAddress().c_str());
  strcpy(softap_mac, WiFi.softAPmacAddress().c_str());
  Serial.printf("STA MAC: %s\r\n", sta_mac);
  Serial.printf(" AP MAC: %s\r\n", softap_mac);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);

  rtc = new CMMC_RTC();
  lcd = new CMMC_LCD();
  xTaskCreate(lcdTask, "lcdTask", 10000, /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    2,                /* Priority of the task. */
                    NULL);

  dustSensor = new CMMC_DustSensor(&Serial1);

  modules[0] = dustSensor;
  modules[1] = new CMMC_GPS(&Serial1);

  for (size_t i = 1; i < MODULE_SIZE; i++) {
    modules[i]->setup();
  }

  xTaskCreate(nbTask, "TaskTwo", 10000, /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);
}

void nbTask(void * parameter)
{
    NBSerial.begin(9600, SERIAL_8N1, 26 /*rx*/, 27 /* tx */);
    NBSerial.setTimeout(4);
    delay(20);
    modem = new CMMC_Modem(&NBSerial);
    modem->setup();
    while (1) {
      modem->loop();
      delay(10);
    }
    Serial.println("Ending task 2");
    vTaskDelete( NULL );
}

void lcdTask(void * parameter)
{
    lcd->setup();
    rtc->setup();
    int element;
    pinMode(EXT_WDT_PIN, OUTPUT);
    while (1) {
      rtc->loop();
      lcd->loop();
      if (modem != NULL) {
        if (modem->lastSentOkMillis > 0) {
          uint32_t lastSentInSeconds = (millis() - modem->lastSentOkMillis)/1000;
          Serial.println(lastSentInSeconds);
          if (lastSentInSeconds > 120) {
            Serial.println("FFFF.. CAUSE EXTERNAL RST.");
          }
          else {
            digitalWrite(EXT_WDT_PIN, HIGH);
            digitalWrite(EXT_WDT_PIN, LOW);
          }
        }
        else {
          digitalWrite(EXT_WDT_PIN, HIGH);
          digitalWrite(EXT_WDT_PIN, LOW);
        }
      }
      // BaseType_t xStatus;
      // const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
      // xStatus = xQueueReceive( xQueueMain, &element, xTicksToWait );
      // /* check whether receiving is ok or not */
      // if(xStatus == pdPASS){
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      //   Serial.println("[LCD TASK] QUEUE RECV...");
      // }
    }
    Serial.println("Ending LCD Task");
    vTaskDelete( NULL );
}


void loop() {
  for (size_t i = 0; i < MODULE_SIZE; i++) {
    modules[i]->loop();
  }
  // Serial.printf("rebootCount = %lu\r\n", rebootCount);
}


void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : Serial.println ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
    case 3 : Serial.println ("SW_RESET");break;               /**<3,  Software reset digital core*/
    case 4 : Serial.println ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : Serial.println ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : Serial.println ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : Serial.println ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : Serial.println ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : Serial.println ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : Serial.println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : Serial.println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : Serial.println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : Serial.println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : Serial.println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : Serial.println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : Serial.println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Serial.println ("NO_MEAN");
  }
}

void verbose_print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1  : Serial.println ("Vbat power on reset");break;
    case 3  : Serial.println ("Software reset digital core");break;
    case 4  : Serial.println ("Legacy watch dog reset digital core");break;
    case 5  : Serial.println ("Deep Sleep reset digital core");break;
    case 6  : Serial.println ("Reset by SLC module, reset digital core");break;
    case 7  : Serial.println ("Timer Group0 Watch dog reset digital core");break;
    case 8  : Serial.println ("Timer Group1 Watch dog reset digital core");break;
    case 9  : Serial.println ("RTC Watch dog Reset digital core");break;
    case 10 : Serial.println ("Instrusion tested to reset CPU");break;
    case 11 : Serial.println ("Time Group reset CPU");break;
    case 12 : Serial.println ("Software reset CPU");break;
    case 13 : Serial.println ("RTC Watch dog Reset CPU");break;
    case 14 : Serial.println ("for APP CPU, reseted by PRO CPU");break;
    case 15 : Serial.println ("Reset when the vdd voltage is not stable");break;
    case 16 : Serial.println ("RTC Watch dog reset digital core and rtc module");break;
    default : Serial.println ("NO_MEAN");
  }
}
