#include <Arduino.h>
#include <CMMC_GPS.h>
#include <CMMC_LCD.h>
#include <CMMC_NB_IoT.h>
#include <HardwareSerial.h>
#define GPSSerial Serial1

// HardwareSerial NBSerial(2);
// CMMC_NB_IoT nb(&Serial1);

CMMC_LCD *lcd;
CMMC_GPS *gps;

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  Serial2.begin(9600, SERIAL_8N1, 27 /*rx*/, 26 /* tx */);

  gps = new CMMC_GPS(&GPSSerial);
  lcd = new CMMC_LCD();
  Serial2.setTimeout(4);
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
  gps->loop();
  String at = "AT";
  // Serial2.write(at.c_str(), 2);
  // Serial2.write('\r');
  // String response =  Serial2.readStringUntil('\n');
  // Serial.println(response);
}
