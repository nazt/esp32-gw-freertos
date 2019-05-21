
#include <CMMC_Legend.h>
#include <Preferences.h>
// #include "modules/LCDModule.h"

const char* MEOBOT_VERSION = "1.5";

CMMC_Legend os;

os_config_t config = {
  .baudrate = 115200,
  .blink_gpio = 21,
  .button1_gpio = 0,
  .sw_mode_logic = LOW,
  .button1_gpio_mode = INPUT_PULLUP,
  .delay_after_init_ms = 200,
};

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
