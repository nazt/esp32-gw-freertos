#include "CMMC_GPS.h"

// extern CMMC_RTC* rtc;


CMMC_GPS::CMMC_GPS(HardwareSerial *s) {
  this->serial = s;
}

void CMMC_GPS::configLoop() {
  yield();
}

String CMMC_GPS::getLocation() {
  return String(_bufferLatLng);
  // char x[100];
  // strcpy(x, this->_bufferLatLng);
  // sprintf(x, "%s", this->_bufferLatLng);
}

void CMMC_GPS::configSetup() {
  yield();
}

void CMMC_GPS::setup() {
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
    if (millis() - ms > 2000) {
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
        RawDegrees lat = gps.location.rawLat();
        RawDegrees lng = gps.location.rawLng();
        sprintf(latC, "%s%u.%lu", lat.negative ? "-" : "", lat.deg, lat.billionths);
        sprintf(lngC, "%s%u.%lu", lng.negative ? "-" : "", lng.deg, lng.billionths);
        strcpy(latlngC, "");
        strcat(latlngC, latC);
        strcat(latlngC, ",");
        strcat(latlngC, lngC);
        strcpy(_bufferLatLng, latlngC);
        // Serial.println(gps.location.lat(), 6); // Latitude in degrees (double)
        // Serial.println(gps.location.lng(), 6); // Longitude in degrees (double)
        // Serial.print(gps.location.rawLat().negative ? "-" : "+");
        // Serial.println(gps.location.rawLat().deg); // Raw latitude in whole degrees
        // Serial.println(gps.location.rawLat().billionths);// ... and billionths (u16/u32)
        // Serial.print(gps.location.rawLng().negative ? "-" : "+");
        // Serial.println(gps.location.rawLng().deg); // Raw longitude in whole degrees
        // Serial.println(gps.location.rawLng().billionths);// ... and billionths (u16/u32)
        // Serial.println(gps.date.value()); // Raw date in DDMMYY format (u32)
        // Serial.println(gps.date.year()); // Year (2000+) (u16)
        // Serial.println(gps.date.month()); // Month (1-12) (u8)
        // Serial.println(gps.date.day()); // Day (1-31) (u8)
        // Serial.println(gps.time.value()); // Raw time in HHMMSSCC format (u32)
        // Serial.println(gps.time.hour()); // Hour (0-23) (u8)
        // Serial.println(gps.time.minute()); // Minute (0-59) (u8)
        // Serial.println(gps.time.second()); // Second (0-59) (u8)
        // Serial.println(gps.time.centisecond()); // 100ths of a second (0-99) (u8)
        // Serial.println(gps.speed.value()); // Raw speed in 100ths of a knot (i32)
        // Serial.println(gps.speed.knots()); // Speed in knots (double)
        // Serial.println(gps.speed.mph()); // Speed in miles per hour (double)
        // Serial.println(gps.speed.mps()); // Speed in meters per second (double)
        // Serial.println(gps.speed.kmph()); // Speed in kilometers per hour (double)
        // Serial.println(gps.course.value()); // Raw course in 100ths of a degree (i32)
        // Serial.println(gps.course.deg()); // Course in degrees (double)
        // Serial.println(gps.altitude.value()); // Raw altitude in centimeters (i32)
        // Serial.println(gps.altitude.meters()); // Altitude in meters (double)
        // Serial.println(gps.altitude.miles()); // Altitude in miles (double)
        // Serial.println(gps.altitude.kilometers()); // Altitude in kilometers (double)
        // Serial.println(gps.altitude.feet()); // Altitude in feet (double)
        // Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
        // Serial.println(gps.hdop.value()); // Horizontal Dim. of Precision (100ths-i32)
        // Serial.println()
        this->_lastFetchLocation = millis();
        if (gps.time.isUpdated()) {
          this->_lastSyncRtc = millis();
          _dt =  DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour()+7%24, gps.time.minute(), gps.time.second());
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

DateTime CMMC_GPS::getDateTime() {
  return _dt;
}
