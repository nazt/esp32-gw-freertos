#include <Arduino.h>
#include <CMMC_GPS.h>
#include <CMMC_LCD.h>

#define GPSSerial Serial1

CMMC_LCD *lcd;

CMMC_GPS *gps;

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  Serial.println("HELLO..");
  gps = new CMMC_GPS(&GPSSerial);
  lcd = new CMMC_LCD();

  CMMC_Module* modules[10];
  modules[0] = lcd;
  modules[1] = gps;
  // lcd->setup(); // gps->setup();
  for (size_t i = 0; i < 2; i++) {
    modules[i]->setup();
  }
}

void processGps() {
}


void loop() {
  // gps->loop();
  lcd->loop();
}
