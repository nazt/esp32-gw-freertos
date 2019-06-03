#ifndef CMMC_LCD_H
#define CMMC_LCD_H

#include <defs.h>

#include <Arduino.h>
#include <CMMC_Module.h>
#include <U8g2lib.h>
#include <CMMC_Modem.h>
// #include <CMMC_RTC.h>

class CMMC_LCD: public CMMC_Module{
  uint32_t counter = 0;
  uint32_t sentCnt = 0;
  uint8_t peerCount = 0;
  int packetRecv = 0;
  uint8_t page = 0;
  const int MAX_PAGE = 3;

  U8G2_ST7920_128X64_F_SW_SPI *u8g2;
  float  batt;
  float  batt_raw;
  float  batt_percent;
  int analogValue;
  bool gpsNoSignal = 1;
  int rebootCount = 0;
  int nbSentOk = 0;
  int keepAliveSent = 0;
  int pArrIdx = 0;
  int status = 0;
  String latC;
  String lngC;
  String sta_mac;
  String softap_mac;
  const int logoMargin = 36;
  const int marginLeft = 6;
  char _ap_name[20];
  protected:
    // char path[20];
    // String saveConfig(AsyncWebServerRequest *request, CMMC_ConfigManager* configManager);
  public:
    float pm10 = 0;
    float pm2_5 = 0;
    CMMC_LCD();
    void config(CMMC_System *os, AsyncWebServer* server);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
    void displayLogo();
    void paintRunPage();
    void paintDetailPage();
    void paintConfigPage();
    void paintLongPressedButton();
    void paintPressedButton();
    void setApName(char*);


    const char* formatedNumber(char* buffer, int n);
    const char* name() {
      return "CMMC_LCD";
    }
};

#endif
