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
