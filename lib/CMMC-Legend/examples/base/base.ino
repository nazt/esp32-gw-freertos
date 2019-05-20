#include <Arduino.h>
#include <CMMC_Legend.h>

CMMC_Legend os;

void setup()
{
  os.setup();
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  os.run();
}