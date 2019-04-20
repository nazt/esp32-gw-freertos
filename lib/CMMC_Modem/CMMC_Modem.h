#ifndef CMMC_Modem_H
#define CMMC_Modem_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <CMMC_NB_IoT.h>
#include <TinyGPS++.h>
#include <CMMC_GPS.h>

typedef enum {
  TYPE_KEEP_ALIVE = 1,
  TYPE_SENSOR_NODE
} DATA_COAP_TYPE;

extern char sta_mac[18];
extern char softap_mac[18];

typedef struct{
  float  batt = 0;
  float  batt_raw = 0;
  float  batt_percent = 0;
  float pm10;
  float pm2_5;
  int analogValue;
  uint32_t uptime_s;
  uint32_t unixtime;
  uint32_t rebootCount = 0;
  char latlngC[80];
  // DateTime dt;
  unsigned int ct = 1;
  DATA_COAP_TYPE packet_type;
} Data;

class CMMC_Modem: public CMMC_Module{
  protected:
    CMMC_NB_IoT *nb;
    Stream *_modemSerial;
    int isNbConnected = 0;
    String status;
    uint32_t nbSentCounter = 1;
    void updateStatus(String s);
    // void resetModem();
    void sendPacket(uint8_t *text, int buflen);

    CMMC_Interval keepAliveInterval;
    xQueueHandle xQueue;
    void receiveTask( void * parameter  );
  public:
    CMMC_Modem(Stream*);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
    String getStatus();
    uint32_t lastSentOkMillis = 0;
};

#endif
