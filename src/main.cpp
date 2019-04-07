#include <Arduino.h>
#include <CMMC_GPS.h>
#include <CMMC_LCD.h>
#include <CMMC_NB_IoT.h>
#include <HardwareSerial.h>
#include <CMMC_Modem.h>
#include <CMMC_DustSensor.h>

HardwareSerial GPSSerial(1);
HardwareSerial NBSerial(2);
HardwareSerial DustSerial(3);

CMMC_LCD *lcd;
CMMC_GPS *gps;
CMMC_Modem *modem;

const int MODULE_SIZE = 3;
CMMC_Module* modules[10];

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 12 /*rx*/, 15 /* tx */);
  NBSerial.begin(9600, SERIAL_8N1, 27 /*rx*/, 26 /* tx */);
  DustSerial.begin(9600, SERIAL_8N1, 32/*rx*/, 33 /* tx */);
  NBSerial.setTimeout(4);

  modules[0] = lcd = new CMMC_LCD();
  modules[1] = new CMMC_GPS(&GPSSerial);
  modules[2] = new CMMC_Modem(&NBSerial);

  for (size_t i = 0; i < MODULE_SIZE; i++) {
    modules[i]->setup();
  }
}

void processGps() {

}

void loop() {
  for (size_t i = 0; i < MODULE_SIZE; i++) {
    modules[i]->loop();
  }
  // NBSerial.write(at.c_str(), 2);
  // NBSerial.write('\r');
  // String response =  NBSerial.readStringUntil('\n');
  // Serial.println(response);
}
