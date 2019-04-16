#include "CMMC_LCD.h"
#include "logo.h"
#include <CMMC_Modem.h>
#include <CMMC_RTC.h>
#include <CMMC_DustSensor.h>
// extern CMMC_GPS *gps;
// extern CMMC_GPS *gps;
extern CMMC_Modem *modem;
extern CMMC_RTC *rtc;
extern CMMC_DustSensor *dustSensor;

String pm10Value = "999.88";
String pm2_5Value = "45.49";

CMMC_LCD::CMMC_LCD() {
}

void CMMC_LCD::configLoop() {
  yield();
}

void CMMC_LCD::configSetup() {
  yield();
}

void CMMC_LCD::setup() {
  u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, /* clock=*/ 18, /* MOSI=*/ 23, /* MISO=*/ 19);
  u8g2->begin();
  hello();
}

void CMMC_LCD::loop() {
  paintScreen();
}

const char* CMMC_LCD::formatedNumber(char* buffer, int n) {
  sprintf(buffer, "%3d", n);
  return buffer;
}

void CMMC_LCD::hello() {
  // u8g2->clearBuffer();          // clear the internal memory
  // u8g2->setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  // // u8g2->drawStr(40, 10, "Position");
  // u8g2->drawStr(0, 20, "It's a test.");
  // u8g2->sendBuffer();       latC   // transfer internal memory to the display
  this->displayLogo();
  delay(1000);
}

void CMMC_LCD::paintScreen() {
  u8g2->firstPage();
    int factor = micros()%6;
    factor = +0;
    do
    {
      u8g2->setFont(u8g2_font_p01type_tn);
      u8g2->setCursor(50, 6);
      u8g2->print(rtc->getDateTimeString());

      int marginLeft = 6;
      if (page == 0) {
        int logoMargin = 40;
        // u8g2->drawXBM(0,0,128,64, logo);
        u8g2->drawXBM(5, 5, 40, 32, cat);

        u8g2->setFont(u8g2_font_siji_t_6x10);
        u8g2->setCursor(logoMargin+10, 16+1);
        u8g2->print("DustBoy 2.0");

        u8g2->setFont(u8g2_font_siji_t_6x10);
        u8g2->setCursor(logoMargin+10, 27+1);
        u8g2->print("NB-IoT");

        u8g2->setFont(u8g2_font_micro_tr);
        u8g2->setCursor(logoMargin+48, 27+1);
        u8g2->print(modem->getStatus());
        u8g2->setFont(u8g2_font_logisoso16_tf);
        u8g2->setFont(u8g2_font_siji_t_6x10);
        u8g2->setCursor(marginLeft, 50);
        u8g2->print("PM10");
        u8g2->setCursor(marginLeft, 62);
        u8g2->print("PM2.5");

        u8g2->setCursor(marginLeft+35, 50);
        u8g2->print(String(dustSensor->getPMValue(DustPM10)));
        u8g2->setCursor(marginLeft+35, 62);
        u8g2->print(String(dustSensor->getPMValue(DustPM2_5)));

        u8g2->setCursor(marginLeft+75, 50);
        u8g2->print("ug/m3");
        u8g2->setCursor(marginLeft+75, 62);
        u8g2->print("ug/m3");
        // u8g2->print("%");
      }
      else if (page == 1) {
      }
      else if (page == 2) {
      }
        // sensorModule->_pin0StateDirty = false;
    } while (u8g2->nextPage());
}
void CMMC_LCD::displayLogo() {
  u8g2->firstPage();
  do
  {
    // u8g2->drawXBM(0, 0, 40, 32, cat);
    u8g2->setFont(u8g2_font_logisoso16_tr);
    u8g2->setCursor(44, 24);
    u8g2->setCursor(10  , 26);
    u8g2->print("DustBoy 2.0");
    u8g2->setCursor(30, 50);
    u8g2->print("NB-IoT");
    u8g2->setCursor(40, 46);
    u8g2->setFont(u8g2_font_10x20_te);
    // u8g2->print("Starting...");
  } while (u8g2->nextPage());
}
