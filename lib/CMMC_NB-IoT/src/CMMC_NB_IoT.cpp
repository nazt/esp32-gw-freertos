#include "CMMC_NB_IoT.h"

#define debugPrintLn(...) { if (!this->_disableDiag && this->_diagStream) this->_diagStream->println(__VA_ARGS__); }
#define debugPrint(...) { if (!this->_disableDiag && this->_diagStream) this->_diagStream->print(__VA_ARGS__); }
// #ifdef CMMC_DEBUG
// #warning "Debug mode is ON"
// #else
// #define debugPrintLn(...)
// #define debugPrint(...)
// #endif

#define TIMEOUT_5s 5
#define TIMEOUT_10s 10
#define SILENT 1

void array_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}

CMMC_NB_IoT::CMMC_NB_IoT(Stream *s) {
  this->_modemSerial = s;
  this->_user_onDeviceReboot_cb = [](void) -> void { };
  this->_user_onConnecting_cb = [](void) -> void { };
  this->_user_onConnected_cb = [](void) -> void { };
  this->_user_onDeviceReady_cb = [](void) -> void { };
  this->_user_onDeviceInfo_cb = [](DeviceInfo d) -> void { };
  this->_socketsMap = HashMap<String, Udp*, HASH_SIZE>();
  this->_deviceNeverConnected = true;
};

CMMC_NB_IoT::~CMMC_NB_IoT() {

};

bool CMMC_NB_IoT::callCommand(String at, uint8_t timeout, int retries, char *outStr) {
  int r = 0;
  int ok = false;

  while (r < retries && !ok) {
    if (this->_writeCommand(at.c_str(), timeout, outStr) == 1) {
      ok = true;
    }
    else {
      r++;
      delay(300);
    }
    delay(10);
  }
  return ok;
}

void CMMC_NB_IoT::activate() {
  while (!callCommand(F("AT+CGATT=1"), TIMEOUT_10s, 100));
}

void CMMC_NB_IoT::rebootModule() {
  while (!callCommand(F("AT+NRB"), TIMEOUT_5s));
  _user_onDeviceReboot_cb();
}

void CMMC_NB_IoT::hello() {
  while (!callCommand(F("AT"), TIMEOUT_5s));
}

bool CMMC_NB_IoT::setPhoneFunctionality(unsigned int fun) {
  char buf[12] = {0};
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "AT+CFUN=%u", fun);
  return callCommand(buf, TIMEOUT_5s, 5);
}

char* CMMC_NB_IoT::getDeviceIP() {
  char buffer[100];
  while (!callCommand(F("AT+CGPADDR"), TIMEOUT_5s, 5, buffer));
  return buffer;
}

void CMMC_NB_IoT::queryDeviceInfo() {
  while (!callCommand(F("AT+CGSN=1"), TIMEOUT_5s, 5, this->deviceInfo.imei));
  while (!callCommand(F("AT+CGMR"), TIMEOUT_5s, 5, this->deviceInfo.firmware));
  while (!callCommand(F("AT+CIMI"), TIMEOUT_5s, 5, this->deviceInfo.imsi));
  this->_user_onDeviceInfo_cb(this->deviceInfo);
}

void CMMC_NB_IoT::begin(Stream *s, uint8_t timeout) {
  if (s != 0) {
    this->_modemSerial = s;
  }
  s->setTimeout(timeout);
  debugPrintLn("Debug mode is ON");
  while (!callCommand(F("AT"), TIMEOUT_10s));
  this->_user_onDeviceReady_cb();
}

void CMMC_NB_IoT::loop() {
  char buf[100];
  // this->_loopTimer.every_ms(1000, []() { });
  if (this->_deviceNeverConnected) {
    this->_writeCommand(F("AT+CGATT?"), 10L, buf);
    unsigned int nbNetworkConnected;
    sscanf(buf, "+CGATT:%u", &nbNetworkConnected);
    if (nbNetworkConnected) {
      this->_deviceNeverConnected = false;
      this->_user_onConnected_cb();
    }
    else {
      this->_user_onConnecting_cb();
    }
  } else { // device connected.
    uint8_t socketId;
    int dataLen;
    char ip[16]; // max IP size 4*3 digits + 3 dots + zero term = 16
    uint16_t port;
    uint16_t length;
    uint16_t remainingLength;
    char buf2[100];
    char data[100];

    if (this->_modemSerial->available()) {
      String response = this->_modemSerial->readStringUntil('\n');
      if (response.length() > 0) {
        response.trim();
        Serial.println("===================");
        Serial.println(response);
        Serial.println(response.length());
        Serial.println("===================");
      }
    }
  }
}

Stream* CMMC_NB_IoT::getModemSerial() {
  return this->_modemSerial;
}

bool CMMC_NB_IoT::sendMessageHex(uint8_t *msg, uint16_t len, uint8_t socketId) {
  return this->_socketsMap.valueAt(socketId)->sendMessageHex(msg, len);
}

int CMMC_NB_IoT::createUdpSocket(String hostname, uint16_t port, UDPConfig c) {
  int idx = this->_socketsMap.size();
  String hashKey = String(hostname + ":" + port);
  char resBuffer[40];
  char buffer[60];
  randomSeed(esp_random());
  sprintf(buffer, "AT+NSOCR=DGRAM,17,%u,0", random(6000, 50000));
  Serial.printf("notification config = %s\n", buffer);
  // callCommand("AT+NSOCL=2");
  // callCommand("AT+NSOCL=1");
  if (callCommand(buffer, 10, 5, resBuffer)) {
    if (!this->_socketsMap.contains(hashKey)) {
      debugPrint("socket id=");
      debugPrint(idx);
      debugPrint(" has been created. len=");
      debugPrintLn(this->_socketsMap.size());
      this->_socketsMap[hashKey] = new Udp(hostname, port, idx, this);
    }
    else {
      debugPrint("existing socket id=");
      debugPrintLn(hashKey);
    }
  }
  else {
    debugPrintLn("Create UDP Socket failed.\n");
    idx = -1;
  }
  return idx;
};

void CMMC_NB_IoT::onMessageArrived(onMessageCb_t cb) {
  this->_user_onMessage_cb = cb;
}

void CMMC_NB_IoT::onDeviceReady(voidCb_t cb) {
  this->_user_onDeviceReady_cb = cb;
}

void CMMC_NB_IoT::onDeviceInfo(deviceInfoCb_t cb) {
  this->_user_onDeviceInfo_cb = cb;
}

void CMMC_NB_IoT::onConnecting(voidCb_t cb) {
  this->_user_onConnecting_cb = cb;
}

void CMMC_NB_IoT::onConnected(voidCb_t cb) {
  this->_user_onConnected_cb = cb;
}

void CMMC_NB_IoT::onDeviceReboot(voidCb_t cb) {
  this->_user_onDeviceReboot_cb = cb;
}

bool CMMC_NB_IoT::_writeCommand(String at, uint32_t timeoutS, char *outStr, bool silent) {
  at.trim();
  uint32_t startMs = millis();
  uint32_t nextTimeout = startMs + (timeoutS * 1000L);
  bool reqSuccess = 0;
  if (!silent) {
    debugPrint(">> ");
    debugPrint(at.c_str());
  }
  this->_modemSerial->write(at.c_str(), at.length());
  this->_modemSerial->write('\r');
  String nbSerialBuffer = "";
  while (1) {
    if (this->_modemSerial->available()) {
      String response = this->_modemSerial->readStringUntil('\n');
      response.trim();
      nbSerialBuffer += response;
      if (response.indexOf(F("OK")) != -1) {
        if (!silent) {
          char timeTextBuffer[12]; // ex: (10000ms)
          sprintf(timeTextBuffer, " (%lums)", millis() - startMs);
          debugPrintLn(timeTextBuffer);
        }
        if (outStr != NULL) {
          strcpy(outStr, nbSerialBuffer.c_str());
        }
        reqSuccess = 1;
        break;
      }
      else if (response.indexOf(F("ERROR")) != -1) {
        debugPrintLn(" ....");
        reqSuccess = 0;
        break;
      }
    }
    if ((millis() > nextTimeout) ) {
      nextTimeout += timeoutS;
      reqSuccess = 0;
      debugPrintLn("Wait timeout..");
      debugPrintLn(nbSerialBuffer.c_str());
      nbSerialBuffer = "@";
      break;
    }
    else {
      delay(10);
    }
  }
  return reqSuccess;
}
