#include "CMMC_LCD.h"
#include "logo.h"
#include <TimeLib.h>

// extern struct shared_pool pool;
extern SCREEN_PAGE xpage;
extern char ap_name[20];

CMMC_LCD::CMMC_LCD() {
  u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, /* clock=*/ 18, /* MOSI=*/ 23, /* MISO=*/ 19);
  u8g2->begin();
  String taskMessage = "[CMMC_LCD] Task running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  strcpy(dateTimeString, "");
}

void CMMC_LCD::config(CMMC_System *os, AsyncWebServer *server)
{

}

void CMMC_LCD::configLoop() {
  yield();
}

void CMMC_LCD::configSetup() {
  yield();
}

void CMMC_LCD::setup() {
}

void CMMC_LCD::setApName(char* ap_name) {
  strcpy(_ap_name, ap_name);
}


void CMMC_LCD::loop() {
  // int state = digitalRead(0);
  // if (state == LOW) {
  //   page = ++page % (MAX_PAGE);
  // }
  Serial.println("lcd.loop()");
  paintScreen();
}

const char* CMMC_LCD::formatedNumber(char* buffer, int n, char* suff) {
  if (suff) {
    sprintf(buffer, "%3d%s", n, suff);
  }
  else {
    sprintf(buffer, "%3d", n);
  }
  return buffer;
}

void CMMC_LCD::hello() {
  this->displayLogo();
}

#include "_lcd_run.hpp"
#include "_lcd_config.hpp"
#include "_lcd_detail.hpp"
#include "_lcd_pressed.hpp"
#include "_lcd_long_pressed.hpp"

void CMMC_LCD::paintScreen() {
  u8g2->clearBuffer();
  page = 0;
  int factor = micros() % 6;
  factor = +0;
    u8g2->setFont(u8g2_font_p01type_tn);
    u8g2->setCursor(60, 6);
    if (xpage == LCD_LOGO) {
      displayLogo();
    }
    else if (xpage == LCD_RUN) {
      paintRunPage();
    }
    else if (xpage == LCD_BUTTON_PRESSED) {
      paintPressedButton();
    }
    else if (xpage == LCD_BUTTON_LONG_PRESSED) {
      paintLongPressedButton();
    }
    else if (xpage == LCD_DETAIL_PACKET) {
      paintDetailPage();
    }
    else if (xpage == LCD_CONFIG) {
      setApName(ap_name);
      paintConfigPage();
    }
    u8g2->sendBuffer();
    delay(10);
}

void CMMC_LCD::displayLogo() {
    // u8g2->drawXBM(0, 0, 40, 32, cat);
    u8g2->setFont(u8g2_font_logisoso16_tr);
    u8g2->setCursor(44, 24);
    u8g2->setCursor(10  , 26);
    u8g2->print("DustBoy 2.0");
    u8g2->setCursor(30, 50);
    u8g2->print("NB-IoT");
    u8g2->setCursor(40, 46);
    u8g2->setFont(u8g2_font_10x20_te);
    u8g2->sendBuffer();
}
