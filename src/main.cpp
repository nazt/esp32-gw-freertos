
#include <CMMC_Legend.h>
#include <Preferences.h>

const char* MEOBOT_VERSION = "1.5";

CMMC_Legend os;

os_config_t config = {
  .baudrate = 115200,
  .blink_gpio = 21,
  .button1_gpio = 38,
  .button1_gpio_mode = INPUT_PULLUP,
  .SWITCH_PRESSED_LOGIC = LOW,
  .delay_after_init_ms = 200
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
