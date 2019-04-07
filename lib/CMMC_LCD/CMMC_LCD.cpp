#include "CMMC_LCD.h"


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
  hello();
  // paintScreen();
}

const char* formatedNumber(char* buffer, int n) {
  sprintf(buffer, "%3d", n);
  return buffer;
}

void CMMC_LCD::hello() {
  u8g2->clearBuffer();          // clear the internal memory
  u8g2->setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  // u8g2->drawStr(40, 10, "Position");
  u8g2->drawStr(0, 20, "It's a test.");
  u8g2->sendBuffer();          // transfer internal memory to the display
}

void CMMC_LCD::paintScreen() {
  // Serial.printf("page = %d\r\n", page);
  if (page == 0) {
    u8g2->firstPage();
    int factor = micros() % 6;
    factor = +0;
    static int count = 0;
    count++;
    do {
      int _pageIdx = 0;
      int marginLeft = 6;
      if (_pageIdx == 0) {
        int logoMargin = 40;
        int lineSpacing = 2;
        char numBuffer[20];
        u8g2->setFont(u8g2_font_micro_tr);
        // u8g2->setCursor(logoMargin+10, 16);
        u8g2->setCursor(5, 10);
        u8g2->print("Local Packet Recv");
        u8g2->setCursor(80 + 5, 10);
        u8g2->print(formatedNumber(numBuffer, packetRecv));

        u8g2->setCursor(5, 15 + (1 * lineSpacing));
        u8g2->print("Keep Alive Packet");
        u8g2->setCursor(80 + 5, 15 + (1 * lineSpacing));
        // u8g2->print(formatedNumber(numBuffer, keepAliveSent));


        // u8g2->setCursor(5, 20 + (2 * lineSpacing));
        // u8g2->print("NB-IoT Packet Sent");
        // u8g2->setCursor(80 + 5, 20 + (2 * lineSpacing));
        // u8g2->print(formatedNumber(numBuffer, nbSentOk));
        //
        // u8g2->setCursor(5, 25 + (3 * lineSpacing));
        // u8g2->print("Reboot");
        // u8g2->setCursor(80 + 5, 25 + (3 * lineSpacing));
        // u8g2->print(formatedNumber(numBuffer, rebootCount));
        //
        // u8g2->setCursor(5, 30 + (4 * lineSpacing));
        // u8g2->print("uptime ");
        //
        // char uptimeBuffer[40];
        // // sprintf(uptimeBuffer, "%dd,%02dh,%02dm,%02ds", day() - 1, hour(), minute(), second());
        // u8g2->setCursor(40 + 5, 30 + (4 * lineSpacing));
        // u8g2->print(uptimeBuffer);


        char statusBuffer[60];

        // sprintf(statusBuffer, "Status: (Queue=%d)", pArrIdx);
        u8g2->setCursor(5, 40 + (5 * lineSpacing));
        u8g2->print(statusBuffer);

        u8g2->setCursor(5, 45 + (6 * lineSpacing));
        // u8g2->print(status);

        // u8g2->setFont(u8g2_font_p01type_tn);
        // u8g2->setFont(u8g2_font_micro_tr);

        // u8g2->setFont(u8g2_font_unifont_t_symbols);
        // // https://github.com/olikraus/u8g2/wiki/u8g2reference
        u8g2->drawGlyph(0 + 4, 10, 9680 + count % 7);
        // u8g2->setCursor(0, 7);
      }
    } while (u8g2->nextPage());
  }
  else if (page == 1) {
    u8g2->firstPage();
    do {
      u8g2->setFont(u8g2_font_micro_tr);
      u8g2->setCursor(5, 10);
      // u8g2->print("It's a swiss arm");
      u8g2->setCursor(5, 20);
      if (!gpsNoSignal) {
        // u8g2->print(latC);
        // u8g2->print(",");
        // u8g2->print(lngC);
      }
      else {
        if (millis() % 2) {
          u8g2->print("Locating....");
        }
        else {
          u8g2->print("Locating.");
        }
      }

      u8g2->setCursor(5, 30);
      u8g2->print("STA MAC ");
      // u8g2->print(sta_mac);

      u8g2->setCursor(5, 40);
      u8g2->print(" AP MAC ");
      // u8g2->print(softap_mac);

      u8g2->setCursor(5, 50);
      u8g2->print("PEER=");
      // u8g2->print(peerCount);
      u8g2->print(" ");
      // u8g2->setCursor(5, 60);
      u8g2->print("MSG QUEUE=");
      // u8g2->print(pArrIdx);

      u8g2->setCursor(5, 60);
      u8g2->print("ANALOG=");
      u8g2->print(analogValue);
      u8g2->print(" ");
      u8g2->print("BATT=");
      u8g2->print(batt);
      u8g2->print("v");
      u8g2->print("(R=");
      u8g2->print(batt_raw);
      u8g2->print(")");

    } while (u8g2->nextPage());
  }
}
