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

const int MODULE_SIZE = 3;

CMMC_Module* modules[10];
void nbTask( void * parameter);

void setup() {
  Serial.begin(115200);
  lcd = new CMMC_LCD();
  lcd->setup();
  lcd->hello();

  xTaskCreate(nbTask, "TaskTwo", 10000, /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);

  modules[0] = lcd;
  modules[1] = new CMMC_GPS(&Serial1);
  modules[2] = new CMMC_DustSensor(&Serial1);

  for (size_t i = 1; i < MODULE_SIZE; i++) {
    modules[i]->setup();
  }

  lcd->hello();
  delay(2000);
}

void nbTask(void * parameter)
{
    NBSerial.begin(9600, SERIAL_8N1, 26 /*rx*/, 27 /* tx */);
    NBSerial.setTimeout(4);
    modem = new CMMC_Modem(&NBSerial);
    modem->setup();
    while (1) {
      modem->loop();
      delay(10);
    }
    Serial.println("Ending task 2");
    vTaskDelete( NULL );
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
