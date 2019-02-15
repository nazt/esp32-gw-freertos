#include "CMMC_GPS.h"


CMMC_GPS::CMMC_GPS(Stream *s) {
  this->serial = s;
}

void CMMC_GPS::configLoop() {
  yield();
}

void CMMC_GPS::configSetup() {
  yield();
}

void CMMC_GPS::setup() {
  Serial.println("SETING UP FROM GPS..");
}

void CMMC_GPS::loop() {
  while (serial->available() > 0) {
    char c = serial->read();
    if (gps.encode(c)) {
      if (gps.location.isValid()) {
        gpsNoSignal = 0;
        RawDegrees lat = gps.location.rawLat();
        RawDegrees lng = gps.location.rawLng();
        sprintf(latC, "%s%u.%lu", lat.negative ? "-" : "", lat.deg, lat.billionths);
        sprintf(lngC, "%s%u.%lu", lng.negative ? "-" : "", lng.deg, lng.billionths);
        strcpy(latlngC, "");
        strcat(latlngC, latC);
        strcat(latlngC, ",");
        strcat(latlngC, lngC);
      }
      else
      {
        gpsNoSignal = 1;
        strcpy(latlngC, "");
        strcat(latlngC, "0.0000");
        strcat(latlngC, ",");
        strcat(latlngC, "0.0000");
        Serial.println(F("INVALID GPS DATA"));
      }
    }
  }
}
