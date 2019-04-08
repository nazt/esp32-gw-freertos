#include "CMMC_DustSensor.h"
#include "utils.hpp"
CMMC_DustSensor::CMMC_DustSensor(HardwareSerial* s)   {
  this->_serial = s;
}

void CMMC_DustSensor::configLoop() {
  yield();
}

void CMMC_DustSensor::configSetup() {
  yield();
}

void CMMC_DustSensor::setup() {

}

void CMMC_DustSensor::loop() {
  this->_serial->begin(9600, SERIAL_8N1, 33 /*rx*/, 32 /* tx */);
  uint32_t ms = millis();
  // Serial.printf("COUNT=%lu\r\n", ms);
  while(!this->_serial->available()) {
    // if (millis() - ms > 2000) {
    //   break;
    // }
    delay(1);
  }
  Serial.printf("wait DustSensor_SERIAL for %lums\r\n", millis() - ms);
  delay(200);
  this->readDustSensor();
}

unsigned int dust_counter = 0;
void CMMC_DustSensor::readDustSensor() {
  uint8_t mData = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;

  while ( this->_serial->available() > 0 ) {
    for ( int i = 0; i < 10; ++i ) {
      mPkt[i] = this->_serial->read();
      // Serial.println( mPkt[i], HEX );
    }
    if ( 0xC0 == mPkt[1] ) {
      // Read dust density.
      // Check
      uint8_t sum = 0;
      for ( int i = 2; i <= 7; ++i ) {
        sum += mPkt[i];
      }
      if ( sum == mPkt[8] ) {
        uint8_t pm25Low   = mPkt[2];
        uint8_t pm25High  = mPkt[3];
        uint8_t pm10Low   = mPkt[4];
        uint8_t pm10High  = mPkt[5];

        pm25 = ( ( pm25High * 256.0 ) + pm25Low ) / 10.0;
        pm10 = ( ( pm10High * 256.0 ) + pm10Low ) / 10.0;
        Serial.println("check sum is valid.");
      }
    }

    dustIdx = dust_counter % MAX_ARRAY;

    pm25_array[dustIdx] = pm25;
    pm10_array[dustIdx] = pm10;
    // Serial.printf("pm10=%f\r\n", pm10);

    if (dustIdx < MAX_ARRAY) {
      dust_average25 = median(pm25_array, dustIdx + 1);
      dust_average10 = median(pm10_array, dustIdx + 1);
    }
    else {
      dust_average25 = median(pm25_array, MAX_ARRAY);
      dust_average10 = median(pm10_array, MAX_ARRAY);
    }

    Serial.println(dust_average25);
    Serial.println(dust_average10);


    dust_counter++;
    this->_serial->flush();

  }
}
