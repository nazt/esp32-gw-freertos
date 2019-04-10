#include "CMMC_LCD.h"
#include "logo.h"
#include <CMMC_Modem.h>
#include <CMMC_RTC.h>
// extern CMMC_GPS *gps;
// extern CMMC_GPS *gps;
extern CMMC_Modem *modem;
extern CMMC_RTC *rtc;
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
}

void CMMC_LCD::paintScreen() {
  u8g2->firstPage();
    int factor = micros()%6;
    factor = +0;
    // ntpModule->toggle = !ntpModule->toggle;
    do
    {
      // if (sensorModule->soil_enable) {
      //   u8g2->setFont(u8g2_font_open_iconic_thing_1x_t);
      //   u8g2->drawGlyph(100, 9, 64+8+9);
      // }
      //
      // if (sensorModule->two_temp_sensors) {
      //   u8g2->setFont(u8g2_font_open_iconic_thing_1x_t);
      //   // u8g2->setCursor(110, 6);
      //   u8g2->drawGlyph(90, 9, 64+8+3);
      // }

      // u8g2->setFont(u8g2_font_micro_tr);
      // u8g2->setCursor(50, 7);
      // u8g2->print(modem->getStatus());

      u8g2->setFont(u8g2_font_p01type_tn);
      u8g2->setCursor(50, 6);
      // u8g2->print(ntpModule->getTimeString());
      // u8g2->print("10/04/2560 13:01");
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
        u8g2->setCursor(logoMargin+12, 27+1);
        u8g2->print("NB-IoT");

        u8g2->setFont(u8g2_font_micro_tr);
        u8g2->setCursor(logoMargin+50, 27+1);
        u8g2->print(modem->getStatus());

        // u8g2->setCursor(logoMargin+12, 35+1);
        // u8g2->print("station");

        u8g2->setFont(u8g2_font_logisoso16_tf);
        // u8g2->setCursor(6+marginLeft, 60);

        // u8g2->setFont(u8g2_font_p01type_tn);
        // u8g2->setCursor(40, 7);
        // String latlng = "18.706064,98.981712";
        // u8g2->print(latlng);
        // u8g2->print(modem->getStatus());
        // u8g2->print("30.0-");
        // u8g2->print("°C");

        // u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
        // u8g2->drawGlyph(74, 60, 152);
        // u8g2->setFont(u8g2_font_logisoso16_tf);
        // u8g2->setCursor(marginLeft, 60);
        u8g2->setFont(u8g2_font_siji_t_6x10);
        u8g2->setCursor(marginLeft, 50);
        u8g2->print("PM10");
        u8g2->setCursor(marginLeft, 62);
        u8g2->print("PM2.5");


        u8g2->setCursor(marginLeft+35, 50);
        u8g2->print("999.88");
        u8g2->setCursor(marginLeft+35, 62);
        u8g2->print("45.49");

        u8g2->setCursor(marginLeft+75, 50);
        u8g2->print("ug/m3");
        u8g2->setCursor(marginLeft+75, 62);
        u8g2->print("ug/m3");

        // u8g2->print("%");

      }
      else if (page == 1) {
        u8g2->setFont(u8g2_font_pxplusibmcga_8u);
        u8g2->setCursor(6, 10);
        u8g2->print("HUMIDITY");

        u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
        u8g2->drawGlyph(74, 30+2, 152);
        u8g2->drawGlyph(74, 60, 152);

        u8g2->setFont(u8g2_font_unifont_t_symbols);
        u8g2->setCursor(6, 30-3+2);
        u8g2->print("Sensor 1");

        u8g2->setFont(u8g2_font_unifont_t_symbols);
        u8g2->setCursor(6, 60-3+2-2);
        u8g2->print("Sensor 2");


        u8g2->setFont(u8g2_font_logisoso16_tf);
        u8g2->setCursor(85+marginLeft, 30+2);
        // u8g2->print(sensorModule->getHumidityString(1));
        u8g2->print("%");

        u8g2->setFont(u8g2_font_logisoso16_tf);
        u8g2->setCursor(85+marginLeft, 60);
        // u8g2->print(sensorModule->getHumidityString(2));
        u8g2->print("%");
      }
      else if (page == 2) {
        u8g2->setFont(u8g2_font_pxplusibmcga_8u);
        u8g2->setCursor(2, 10);
        u8g2->print("TEMPERATURE");

        // u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
        // u8g2->drawGlyph(74, 30+2, 152);
        // u8g2->drawGlyph(74, 60, 152);

        u8g2->setFont(u8g2_font_unifont_t_symbols);
        u8g2->setCursor(6, 30-3+2);
        u8g2->print("Sensor 1");

        u8g2->setFont(u8g2_font_unifont_t_symbols);
        u8g2->setCursor(6, 60-3+2-2);
        u8g2->print("Sensor 2");

        u8g2->setFont(u8g2_font_logisoso16_tf);
        u8g2->setCursor(70+marginLeft, 30+2);
        // u8g2->print(sensorModule->getTemperature(1));
        u8g2->setFont(u8g2_font_logisoso16_tf);
        u8g2->print("°C");


        u8g2->setFont(u8g2_font_logisoso16_tf);
        u8g2->setCursor(70+marginLeft, 60);
        // u8g2->print(sensorModule->getTemperature(2));
        u8g2->setFont(u8g2_font_logisoso16_tf);
        u8g2->print("°C");
      }
        // sensorModule->_pin0StateDirty = false;
    } while (u8g2->nextPage());
}
void CMMC_LCD::displayLogo() {
  // u8g2->firstPage();
  // do
  // {
  //   u8g2->drawXBM(0, 0, 64, 64, qr);
  //   // u8g2->setFont(u8g2_font_logisoso16_tr);
  //   // u8g2->setCursor(36, 28);
  //   // u8g2->print("NB-IoT");
  //
  //   // u8g2->setCursor(36, 46);
  //   // u8g2->setFont(u8g2_font_10x20_te);
  //   // u8g2->print("");
  //   // u8g2->print(String("BRIDGE"));
  // } while (u8g2->nextPage());

  u8g2->firstPage();
  do
  {
    // u8g2->drawXBM(0, 0, 40, 32, cat);
    u8g2->setFont(u8g2_font_logisoso16_tr);
    u8g2->setCursor(44, 24);
    // u8g2->print("  Dustboy");

    u8g2->setCursor(10  , 26);
    u8g2->print("DustBoy 2.0");
    u8g2->setCursor(30, 50);
    u8g2->print("NB-IoT");

    u8g2->setCursor(40, 46);
    u8g2->setFont(u8g2_font_10x20_te);
    // u8g2->print("Starting...");
  } while (u8g2->nextPage());
}
