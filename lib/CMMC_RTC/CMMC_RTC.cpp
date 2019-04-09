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
}

void CMMC_RTC::loop() {
}
