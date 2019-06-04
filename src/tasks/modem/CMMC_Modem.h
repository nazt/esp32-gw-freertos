#ifndef CMMC_Modem_H
#define CMMC_Modem_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <CMMC_NB_IoT.h>
#include <TinyGPS++.h>

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
  unsigned int ct = 1;
  DATA_COAP_TYPE packet_type;
} Data;

enum MODEM_TYPE {
    TYPE_AIS_NB_IOT = 1,
    TYPE_TRUE_NB_IOT,
};

class CMMC_Modem: public CMMC_Module{
  protected:
    CMMC_NB_IoT *nb;
    Stream *_modemSerial;
    HardwareSerial *hwSerial;
    int isNbConnected = 0;
    uint32_t nbSentCounter = 1;
    void updateStatus(String s);
    void sendPacket(uint8_t *text, int buflen);
    CMMC_Interval keepAliveInterval;
    void receiveTask( void * parameter  );
    const char* name() {
      return "CMMC_Modem";
    }
    void config(CMMC_System *os, AsyncWebServer* server) {} ;
    MODEM_TYPE _modemType;

  public:
    CMMC_Modem(Stream*, HardwareSerial*, MODEM_TYPE);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
    String getStatus();
    uint32_t lastSentOkMillis = 0;
    String status;
};

#endif
