#include <Arduino.h>
#include <CMMC_GPS.h>
#include <CMMC_LCD.h>
#include <CMMC_NB_IoT.h>
#include <HardwareSerial.h>
#include <CMMC_Modem.h>
#include <CMMC_DustSensor.h>
#include <CMMC_DustSensor.h>

HardwareSerial NBSerial(2);

CMMC_LCD *lcd;
CMMC_GPS *gps;
CMMC_Modem *modem;

const int MODULE_SIZE = 4 ;
CMMC_Module* modules[10];

void setup() {
  Serial.begin(115200);
  NBSerial.begin(9600, SERIAL_8N1, 26 /*rx*/, 27 /* tx */);
  NBSerial.setTimeout(4);

  modules[0] = lcd = new CMMC_LCD();
  modules[1] = new CMMC_Modem(&NBSerial);
  modules[2] = new CMMC_GPS(&Serial1);
  modules[3] = new CMMC_DustSensor(&Serial1);

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
