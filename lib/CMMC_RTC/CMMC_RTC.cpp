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
  if (! rtc->begin()) {
    Serial.println("RTC Fail to initialize.");
  }
  else {
    Serial.println("RTC initialize OK.");
  }
}

String CMMC_RTC::getDateTimeString() {
  // char tmp[60] = "";
  // strcat(tmp, dateTimeCharArrray);
  return String(dateTimeCharArrray);
}

void CMMC_RTC::loop() {
      DateTime now = rtc->now();
      sprintf(dateTimeCharArrray, "%d/%d/%d %d:%d:%d",
      now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second()
    );

      Serial.println(dateTimeCharArrray);

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
}
