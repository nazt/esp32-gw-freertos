#include <Arduino.h>
#include <CMMC_GPS.h>
#include <CMMC_LCD.h>
#include <CMMC_NB_IoT.h>
#include <HardwareSerial.h>
#include <CMMC_Modem.h>


#define GPSSerial Serial1

HardwareSerial NBSerial(2);
CMMC_NB_IoT *nb;

CMMC_LCD *lcd;
CMMC_GPS *gps;

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  NBSerial.begin(9600, SERIAL_8N1, 27 /*rx*/, 26 /* tx */);

  gps = new CMMC_GPS(&GPSSerial);
  lcd = new CMMC_LCD();
  nb = new CMMC_NB_IoT(&NBSerial);
  NBSerial.setTimeout(4);
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
  // NBSerial.write(at.c_str(), 2);
  // NBSerial.write('\r');
  // String response =  NBSerial.readStringUntil('\n');
  // Serial.println(response);
}
