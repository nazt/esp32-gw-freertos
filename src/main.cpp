
#include <CMMC_Legend.h>
// #include "modules/LCDModule.h"

const char* MEOBOT_VERSION = "1.5";

CMMC_Legend os;

os_config_t config = { .baudrate = 115200, .led_gpio = 12 };

void setup()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);

  os.setup(&config);
}

void loop()
{
  os.run();
}
