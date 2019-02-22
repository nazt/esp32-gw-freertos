#include "CMMC_INA219.h"


CMMC_INA219::CMMC_INA219() {
}

void CMMC_INA219::configLoop() {
  yield();
}

void CMMC_INA219::configSetup() {
  yield();
}

void CMMC_INA219::setup() {
  Wire.begin(21, 22);
  // init ina219
  ina219.begin();
  ina219.setCalibration_16V_400mA();
}

void CMMC_INA219::loop() {
}
