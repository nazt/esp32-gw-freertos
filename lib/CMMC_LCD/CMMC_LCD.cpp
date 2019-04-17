#include "CMMC_LCD.h"
#include "logo.h"
#include <CMMC_Modem.h>
#include <CMMC_RTC.h>
#include <CMMC_DustSensor.h>
#include <TimeLib.h>
// extern CMMC_GPS *gps;
// extern CMMC_GPS *gps;
extern CMMC_Modem *modem;
extern CMMC_RTC *rtc;
extern CMMC_DustSensor *dustSensor;

String pm10Value = "999.88";
String pm2_5Value = "45.49";

uint32_t counter = 0;
uint32_t sentCnt = 0;
uint8_t peerCount = 0;
int packetRecv = 0;
int count = 0;
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
  pinMode(0, INPUT_PULLUP);
}

void CMMC_LCD::loop() {
  int state = digitalRead(0);
  if (state == LOW) {
    page = ++page % (MAX_PAGE);
    delay(100);
  }
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
        // u8g2->drawXBM(5, 5, 40, 32, cat);

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
        int logoMargin = 40;
        int lineSpacing = 2;
        char numBuffer[20];
        u8g2->setFont(u8g2_font_micro_tr);
        // u8g2->setCursor(logoMargin+10, 16);
        u8g2->setCursor(5, 15);
        u8g2->print("Local Packet Recv");
        u8g2->setCursor(80 + 5, 15);
        u8g2->print(formatedNumber(numBuffer, packetRecv));

        u8g2->setCursor(5, 20 + (1 * lineSpacing));
        u8g2->print("Keep Alive Packet");
        u8g2->setCursor(80 + 5, 20 + (1 * lineSpacing));
        u8g2->print(formatedNumber(numBuffer, keepAliveSent));


        u8g2->setCursor(5, 25 + (2 * lineSpacing));
        u8g2->print("NB-IoT Packet Sent");
        u8g2->setCursor(80 + 5, 25 + (2 * lineSpacing));
        u8g2->print(formatedNumber(numBuffer, nbSentOk));

        u8g2->setCursor(5, 30 + (3 * lineSpacing));
        u8g2->print("Reboot");
        u8g2->setCursor(80 + 5, 30 + (3 * lineSpacing));
        u8g2->print(formatedNumber(numBuffer, rebootCount));

        u8g2->setCursor(5, 35 + (4 * lineSpacing));
        u8g2->print("uptime ");

        char uptimeBuffer[40];
        sprintf(uptimeBuffer, "%dd,%02dh,%02dm,%02ds", day() - 1, hour(), minute(), second());
        u8g2->setCursor(40 + 5, 35 + (4 * lineSpacing));
        u8g2->print(uptimeBuffer);


        char statusBuffer[60];

        sprintf(statusBuffer, "Status: (Queue=%d)", pArrIdx);
        u8g2->setCursor(5, 40 + (5 * lineSpacing));
        u8g2->print(statusBuffer);

        u8g2->setCursor(5, 45 + (6 * lineSpacing));
        u8g2->print(modem->getStatus());

        // u8g2->setFont(u8g2_font_p01type_tn);
        // u8g2->setFont(u8g2_font_micro_tr);
        // u8g2->setFont(u8g2_font_unifont_t_symbols);
        // // https://github.com/olikraus/u8g2/wiki/u8g2reference
        // u8g2->drawGlyph(0 + 4, 1f0, 9680 + count % 7);
        // u8g2->setCursor(0, 7);
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
