#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CMMC_Interval.h>

CMMC_Interval interval;

void setup()
{
    Serial.begin(115200); 
}

void loop()
{
    interval.every_ms(2000, [&]() {
        Serial.println(millis());
    });
}
