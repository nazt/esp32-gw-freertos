#include <Arduino.h>
#include <CMMC_GPS.h>

#define GPSSerial Serial1

CMMC_GPS *gps;

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  Serial.println("HELLO..");
  gps = new CMMC_GPS(&GPSSerial);
  gps->setup();
}

void processGps() {
}


void loop() {
  gps->loop();
}
