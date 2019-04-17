#ifndef CMMC_Modem_H
#define CMMC_Modem_H

#include <Arduino.h>
#include <CMMC_Module.h>
#include <CMMC_NB_IoT.h>

class CMMC_Modem: public CMMC_Module{
  protected:
    CMMC_NB_IoT *nb;
    Stream *_modemSerial;
    int isNbConnected = 1;
    String status;
    void updateStatus(String s);
    // void resetModem();
    void sendPacket(uint8_t *text, int buflen);

    // CMMC_PACKET_T pArr[60];
    int pArrIdx = 0;
    uint32_t lastSentOkMillis = 0;
    unsigned int ct = 1;
    static char msgId[5];
    IPAddress ip = IPAddress(103, 20, 205, 85);
    uint8_t _buffer[1300];
    float  batt;
    float  batt_raw;
    float  batt_percent;
    int analogValue;
    CMMC_Interval keepAliveInterval;
    char latC[20];
    char lngC[20];
    char latlngC[40];

    uint8_t currentSleepTimeMinuteByte = 30;
    uint32_t msAfterESPNowRecv = millis();

  public:
    CMMC_Modem(Stream*);
    void hello();
    void setup();
    void loop();
    void configLoop();
    void configSetup();
    void paintScreen();
    String getStatus();

};

#endif
