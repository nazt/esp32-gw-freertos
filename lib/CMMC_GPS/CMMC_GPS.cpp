#include "CMMC_GPS.h"


CMMC_GPS::CMMC_GPS(HardwareSerial *s) {
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
  this->serial->begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  this->serial->flush();
  uint32_t ms = millis();
  // Serial.printf("COUNT=%lu\r\n", ms);
  while(!this->serial->available()) {
    delay(1);
    // if (millis() - ms > 2000) {
    //   break;
    // }
  }
  Serial.printf("wait GPS_SERIAL for %lums\r\n", millis() - ms);
  delay(200);
  while (serial->available() > 0) {
    char c = serial->read();
    if (gps.encode(c)) {
      // Serial.printf("year=%d\r\n", gps.date.year()); // Year (2000+) (u16)
      // Serial.printf("month=%d\r\n", gps.date.month()); // Month (1-12) (u8)
      // Serial.printf("day=%d\r\n", gps.date.day()); // Day (1-31) (u8)
      // Serial.printf("%d\r\n", gps.time.value()); // Raw time in HHMMSSCC format (u32)
      // Serial.printf("hour=%d\r\n", gps.time.hour()); // Hour (0-23) (u8)
      // Serial.printf("minute=%d\r\n", gps.time.minute()); // Minute (0-59) (u8)
      // Serial.printf("second=%d\r\n", gps.time.second()); // Second (0-59) (u8)
      // Serial.printf("centisecond=%d\r\n", gps.time.centisecond()); // 100ths of a second (0-99) (u8)

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
        Serial.println(latlngC);
         if (gps.time.isUpdated()) {
            Serial.print(F("TIME:  "));
            Serial.print(gps.time.hour()+7 % 24);
            Serial.print(F(":"));
            Serial.print(gps.time.minute());
            Serial.print(F(":"));
            Serial.println(gps.time.second());
          }
      }
      else
      {
        gpsNoSignal = 1;
        strcpy(latlngC, "");
        strcat(latlngC, "0.0000");
        strcat(latlngC, ",");
        strcat(latlngC, "0.0000");
        Serial.println(F("INVALID GPS DATA"));
        delay(10);
      }
    }
  }
}
