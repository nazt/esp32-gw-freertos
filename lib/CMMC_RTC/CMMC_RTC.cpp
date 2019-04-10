#include "CMMC_RTC.h"
#include "logo.h"
#include <CMMC_Modem.h>

// extern CMMC_GPS *gps;
// extern CMMC_GPS *gps;
extern CMMC_Modem *modem;
CMMC_RTC::CMMC_RTC() {
}

void CMMC_RTC::configLoop() {
  yield();
}

void CMMC_RTC::configSetup() {
  yield();
}

void CMMC_RTC::setup() {
  this->rtc = new RTC_DS1307();
  prev = millis();
  if (!rtc->begin()) {
    _ready = false;
    Serial.println("RTC Fail to initialize.");
  }
  else {
    _ready = true;
    // rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    Serial.println("RTC initialize OK.");
  }
}

String CMMC_RTC::getDateTimeString() {
  // char tmp[60] = "";
  // strcat(tmp, dateTimeCharArrray);
  return String(dateTimeCharArrray);
}

void CMMC_RTC::loop() {
    if(!_ready) {
      Serial.println("RTC FAILED.");
      return;
    }
    if (millis() - prev >= 500) {
      DateTime now = rtc->now();
      sprintf(dateTimeCharArrray, "%02u/%02u/%02u %02u:%02u:%02u",
      now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
      Serial.println(dateTimeCharArrray);
      delay(100);
      // Serial.print(now.year(), DEC);
      // Serial.print('/');
      // Serial.print(now.month(), DEC);
      // Serial.print('/');
      // Serial.print(now.day(), DEC);
      // Serial.print(" (");
      // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      // Serial.print(") ");
      // Serial.print(now.hour(), DEC);
      // Serial.print(':');
      // Serial.print(now.minute(), DEC);
      // Serial.print(':');
      // Serial.print(now.second(), DEC);
      // Serial.println();
      prev = millis();
  }
}


void CMMC_RTC::adjust(const DateTime& dt) {
  rtc->adjust(dt);
}
