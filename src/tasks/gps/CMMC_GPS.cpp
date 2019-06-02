#include "CMMC_GPS.h"

// extern CMMC_RTC* rtc;
// extern HardwareSerial SERIAL0;

CMMC_GPS::CMMC_GPS(HardwareSerial *s) {
  this->serial = s;
}

void CMMC_GPS::configLoop() {
  yield();
}

String CMMC_GPS::getLocationString() {
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
  String taskMessage = "[CMMC_GPS] Task running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  // SERIAL0.println(taskMessage);
}

void CMMC_GPS::loop() {
  this->serial->begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  this->serial->flush();
  uint32_t ms = millis();
  vTaskDelay(20 / portTICK_PERIOD_MS);
  while(!this->serial->available()) {
    vTaskDelay(1 / portTICK_PERIOD_MS);
    if (millis() - ms > 2000) {
      // SERIAL0.println("GPS_SERIAL timeout!!!");
      break;
    }
  }
  // SERIAL0.printf("wait GPS_SERIAL for %lums\r\n", millis() - ms);
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

TinyGPSLocation CMMC_GPS::getLocation() {
  return _location;
}
