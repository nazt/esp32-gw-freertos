#include "CMMC_GPS.h"
#include <CMMC_RTC.h>

extern CMMC_RTC* rtc;


CMMC_GPS::CMMC_GPS(HardwareSerial *s) {
  this->serial = s;
}

void CMMC_GPS::configLoop() {
  yield();
}

String CMMC_GPS::getLocation() {
  Serial.println("GPS getLocation()");
  return String(_bufferLatLng);
  // char x[100];
  // strcpy(x, this->_bufferLatLng);
  // sprintf(x, "%s", this->_bufferLatLng);
}

void CMMC_GPS::configSetup() {
  yield();
}

void CMMC_GPS::setup() {
  Serial.println("SETING UP FROM GPS..");
  strcpy(latlngC, "00.0000,00.0000");
  strcpy(_bufferLatLng, "00.0000,00.0000");
}

void CMMC_GPS::loop() {
  this->serial->begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  this->serial->flush();
  uint32_t ms = millis();
  delay(20);
  while(!this->serial->available()) {
    delay(1);
    if (millis() - ms > 5000) {
      Serial.println("GPS_SERIAL timeout!!!");
      break;
    }
  }
  // Serial.printf("wait GPS_SERIAL for %lums\r\n", millis() - ms);
  while (serial->available() > 0) {
    char c = serial->read();
    if (gps.encode(c)) {
      if (gps.location.isValid()) {
        gpsNoSignal = 0;
        // 5Minute
        if (!_lastSyncRtc || millis() - _lastFetchLocation > 5*60*1000) {
              RawDegrees lat = gps.location.rawLat();
              RawDegrees lng = gps.location.rawLng();
              sprintf(latC, "%s%u.%lu", lat.negative ? "-" : "", lat.deg, lat.billionths);
              sprintf(lngC, "%s%u.%lu", lng.negative ? "-" : "", lng.deg, lng.billionths);
              strcpy(latlngC, "");
              strcat(latlngC, latC);
              strcat(latlngC, ",");
              strcat(latlngC, lngC);
              // Serial.println(latlngC);
              strcpy(_bufferLatLng, latlngC);
              // Serial.println()
              this->_lastFetchLocation = millis();
        }

        if (gps.time.isUpdated()) {
            if (!_lastSyncRtc || millis() - _lastSyncRtc > 5*1000) {
              Serial.println("[[[SYNC TIME WITH GPS]]]!!!");
              rtc->adjust(DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour()+7%24, gps.time.minute(), gps.time.second()));
              delay(200);
              _lastSyncRtc = millis();
            }
        }
      }
      else
      {
        gpsNoSignal = 1;
        strcpy(latlngC, "00.0000,00.0000");
      }
    }
  }
}
