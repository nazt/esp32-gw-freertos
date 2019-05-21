
#include <CMMC_Legend.h>
// #include "modules/LCDModule.h"

const char* MEOBOT_VERSION = "1.5";

CMMC_Legend os;

void setup()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);

  os.setup();
}

void loop()
{
  os.run();
}
