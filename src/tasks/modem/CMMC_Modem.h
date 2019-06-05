#ifndef CMMC_Modem_H
#define CMMC_Modem_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <CMMC_NB_IoT.h>
#include <TinyGPS++.h>

;

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
    CMMC_Interval keepAliveInterval;
    void receiveTask( void * parameter  );
    const char* name() {
      return "CMMC_Modem";
    }
    void config(CMMC_System *os, AsyncWebServer* server) {} ;
    bool _locked = true;

  public:
    CMMC_Modem(Stream*, HardwareSerial*, MODEM_TYPE);
    void sendPacket(uint8_t *text, int buflen);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
    bool isLocked();
    String getStatus();
    uint32_t lastSentOkMillis = 0;
    char status[100];
    MODEM_TYPE _modemType;
};

#endif
