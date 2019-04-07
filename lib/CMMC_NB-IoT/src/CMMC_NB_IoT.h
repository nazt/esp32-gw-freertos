#ifndef CMMC_NB_IoT_H
#define CMMC_NB_IoT_H

#define CMMC_DEBUG 1
#define SERIAL_BUFFER_SIZE 256

#include <Arduino.h>
#include <HashMap.h>
#include "CMMC_Interval.hpp"
#include "CMMC_AT_Bridge.hpp"

typedef void (*voidCb_t)(void);
typedef void (*onMessageCb_t)(char* b, size_t len, uint8_t socketId, char* ip, uint16_t port);

#define HASH_SIZE 7

enum UDPConfig {
  DISABLE_RECV = 0,
  ENABLE_RECV,
};

class Udp;

class CMMC_NB_IoT
{
  public:
    CMMC_AT_Bridge Bridge;
    CMMC_NB_IoT(Stream *s = NULL);

    typedef struct {
      char firmware[180];
      char imei[60];
      char imsi[60];
    } DeviceInfo;

    void setDebugStream(Stream* stream) { _diagStream = stream; }

    typedef void(*deviceInfoCb_t)(DeviceInfo);
    ~CMMC_NB_IoT();
    void begin(Stream *s = 0, uint8_t timeout = 5);
    void onDeviceInfo(deviceInfoCb_t cb);
    void onDeviceReady(voidCb_t cb);
    void onConnecting(voidCb_t cb);
    void onConnected(voidCb_t cb);
    void onDeviceReboot(voidCb_t cb);
    void onMessageArrived(onMessageCb_t cb);
    char* getDeviceIP();
    void activate();
    // void deactivate();
    Stream* getModemSerial();
    int createUdpSocket(String hostname, uint16_t port, UDPConfig config = DISABLE_RECV);
    bool _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false);
    bool sendMessage(uint8_t *msg, size_t len, uint8_t socketId = 0);
    bool callCommand(String at, uint8_t timeout = 10, int retries = 5, char *outStr = NULL);
    bool setPhoneFunctionality(unsigned int fun);
    void queryDeviceInfo();
    void rebootModule();
    bool sendMessageHex(uint8_t *msg, uint16_t len, uint8_t socketId);
    // bool sendMessage(uint8_t *msg, uint16_t len, uint8_t socketId);
    void hello();
    void loop();

  private:
    CMMC_Interval _loopTimer;
    bool _deviceNeverConnected;
    Stream* _diagStream;
    bool _disableDiag;
    DeviceInfo deviceInfo;
    deviceInfoCb_t _user_onDeviceInfo_cb;
    voidCb_t _user_onDeviceReady_cb;
    voidCb_t _user_onDeviceReboot_cb;
    voidCb_t _user_onConnecting_cb;
    voidCb_t _user_onConnected_cb;
    onMessageCb_t _user_onMessage_cb;
    Stream *_modemSerial;
    HashMap<String, Udp*, HASH_SIZE> _socketsMap;
};

   class Udp {
      public:
        Udp(String host, uint16_t port, uint8_t socketId, CMMC_NB_IoT *modem) {
          this->_host = host;
          this->_port = port;
          this->_socketId = socketId;
          this->_modem = modem;
          this->_modemSerial = modem->getModemSerial();
        };

        bool sendMessageHex(uint8_t *payload, uint16_t len) {
          return send(this->_socketId, _host.c_str(), _port, payload, len);
        }

        bool sendMessage(uint8_t *payload, uint16_t len) {
          return send(this->_socketId, _host.c_str(), _port, payload, len);
        }

        bool send(uint8_t socketId, const char* host, uint16_t port, uint8_t *payload, uint16_t len) {
          char buffer[45];
          sprintf(buffer, "AT+NSOST=%d,%s,%u,%d,", socketId, host, port , len);
          this->_modemSerial->write((char*)buffer, strlen(buffer));
          char t[3];
          // Serial.print(buffer);
          // Serial.printf(" len=%d\r\n", len);
          while (len--) {
            uint8_t b = *(payload++);
            sprintf(t, "%02x", b);
            // Serial.print(t);
            this->_modemSerial->write(t, 2);
            delayMicroseconds(1);
          }
          Serial.println();
          this->_modemSerial->write('\r');
          String nbSerialBuffer = "@";
          int ct = 0;
          while (1) {
            if (this->_modemSerial->available()) {
              String response = this->_modemSerial->readStringUntil('\n');
              response.trim();
              nbSerialBuffer += response;
              if (response.indexOf("OK") != -1) {
                return true;
              }
            }
            else {
              // Serial.println("WAIT response...");
              ct++;
              if (ct > 50) {
                return false;
              }
              delay(10);
            }
            delay(0);
          }
        }



        ~Udp() { };
      private:
        CMMC_NB_IoT *_modem;
        Stream *_modemSerial;
        String _host;
        uint16_t _port;
        uint8_t _socketId;
    };

#endif //CMMC_NB_IoT_H
