#ifndef xCMMC_LED_H
#define xCMMC_LED_H
#include <Arduino.h>
#include <Ticker.h>

typedef enum
{
  BLINK_TYPE_TICKER = 1,
  BLINK_TYPE_INTERVAL,
} blink_t;


static const blink_t TYPE_TICKER = BLINK_TYPE_TICKER;
static const blink_t TYPE_INTERVAL = BLINK_TYPE_INTERVAL;


class xCMMC_LED
{
public:
  xCMMC_LED(blink_t type = BLINK_TYPE_TICKER);
  xCMMC_LED(Ticker *ticker);
  xCMMC_LED init(blink_t type = BLINK_TYPE_TICKER);

  void setPin(uint8_t pin);
  void toggle();
  void blink(uint32_t ms, uint8_t pin);
  void detach();
  void blink(uint32_t ms);
  void set(uint8_t status);
  void low();
  void high();

private:
  unsigned int _ledPin = 254;
  Ticker *_ticker;
  Ticker *_ticker2;
  blink_t _type;
  uint8_t state = LOW;
  uint32_t prev_active;
  bool _initialized = false;
};

#endif
