#define DEBUG

#include <Arduino.h>
#include <CMMC_NB_IoT.h>
#include <AltSoftSerial.h>
#include <HardwareSerial.h>
#include <CMMC_RX_Parser.h>

CMMC_NB_IoT nb(&Serial2);
CMMC_RX_Parser parser(&Serial1);
bool flag = 0;
String token = "c7f549d0-1858-11e8-8630-b33e669e2295";
char tokenHex[100];

void str2Hex(const char* text, char* buffer) {
  size_t len = strlen(text);
  for (int i = 0 ;i < len; i++) {
    sprintf(buffer+i*2, "%02x", text[i]);
  }
}
#include <Arduino.h>

  typedef struct __attribute((__packed__)) {
    uint8_t from[6];
    uint8_t to[6];
    uint8_t type = 0;
    uint32_t battery = 0x00;
    uint32_t field1 = 0x00;
    uint32_t field2 = 0x00;
    uint32_t field3 = 0x00;
    uint32_t field4 = 0x00;
    uint32_t field5 = 0x00;
    uint32_t field6 = 0x00;
    uint32_t field7 = 0x00;
    uint32_t field8 = 0x00;
    uint32_t field9 = 0x00;
    uint8_t nameLen = 0x00;
    char sensorName[16];
    uint32_t ms = 0;
    uint32_t sent_ms = 0;
    uint32_t sum = 0;
  } CMMC_SENSOR_DATA_T;

typedef struct __attribute((__packed__)) {
  uint8_t header[2] = {0x7e, 0x7f};
  uint8_t version = 1;
  uint8_t project = 1;
  uint8_t reserved[4]= {0xff,0xff,0xff,0xff};
  uint32_t sleepTime;
  uint32_t ms;
  CMMC_SENSOR_DATA_T data;
  uint32_t sum;
  uint8_t tail[2] = {0x0d, 0x0a};
} CMMC_PACKET_T;



void setup()
{

  Serial.begin(57600);
  Serial2.begin(9600);
  Serial1.begin(9600);
  Serial.setTimeout(4);
  Serial2.setTimeout(4);
  Serial1.setTimeout(4);
  uint8_t bbb[200];
  parser.on_command_arrived([](CMMC_SERIAL_PACKET_T * packet, size_t len) {
    Serial.println();
    Serial.print("on_command_arrived.");
    Serial.print("cmd = ");
    Serial.println(packet->cmd, HEX);
    if (packet->cmd == 0x99) {
      uint8_t target[6];
      memcpy(target, packet->data, 6);
      for (int i =0; i<6;i++) {
        Serial.print(target[i], HEX);
      }
      Serial.println();
    }
    //  CMMC_PACKET_T p;
    //  Serial.println();
    //  Serial.print("len = ");
    //  Serial.println(len);
    //  Serial.println((char*)packet->data);
    char buf[3];
    for (int i = 0; i < len; i++) {
      sprintf(buf, "%02x", packet->data[i]);
      Serial.print(buf);
    }
    Serial.println();
    Serial.println("LEN=");
    Serial.println(packet->len);
    // static CMMC_PACKET_T wrapped;
     CMMC_PACKET_T p2;
     memcpy(&(p2.version), (uint8_t*)packet->data, sizeof(packet->len)-2);
     Serial.println(p2.version);
     Serial.println(p2.project);
     Serial.print("sleep=");
     Serial.println(p2.sleepTime);
     Serial.print("ms=");
     Serial.println(p2.ms);

     Serial.print("field1=");
     Serial.println(p2.data.field1);
     Serial.print("field2=");
     Serial.println(p2.data.field2);

     Serial.println(p2.data.nameLen);
     Serial.println(p2.data.sensorName);
    //  memcpy(&buff, &p2.data, sizeof(p2.data));
    //  Serial.println((p2.data)).field1);
    // memcpy(&wrapped.data, &packet, sizeof(packet));
    // Serial.printf("ON_PARSER at (%lums)", millis());
    // Serial.printf("CMD->0x%2x\r\n", packet->cmd);
    // Serial.printf("LEN->%lu\r\n", packet->len);
  });
  delay(50);
  Serial.println();
  Serial.print(F("Starting Application... at ("));
  Serial.print(millis());
  Serial.println("ms)");

  str2Hex(token.c_str(), tokenHex);
  Serial.println(tokenHex);
  nb.setDebugStream(&Serial);

  nb.onDeviceReboot([]() {
    Serial.println(F("[user] Device rebooted."));
    // nb.queryDeviceInfo();
    // delay(1000);
  }); nb.onDeviceReady([]() {
    Serial.println("[user] Device Ready!");
  });

  nb.onDeviceInfo([](CMMC_NB_IoT::DeviceInfo device) {
    Serial.print(F("# Module IMEI-->  "));
    Serial.println(device.imei);
    Serial.print(F("# Firmware ver-->  "));
    Serial.println(device.firmware);
    Serial.print(F("# IMSI SIM-->  "));
    Serial.println(device.imsi);
  });

  nb.onMessageArrived([](char *text, size_t len, uint8_t socketId, char* ip, uint16_t port) {
    char buffer[100];
    sprintf(buffer, "++ [recv:] socketId=%u, ip=%s, port=%u, len=%d bytes (%lums)", socketId, ip, port, len, millis());
    Serial.println(buffer);
  });

  nb.onConnecting([]() {
    Serial.println("Connecting to NB-IoT...");
    delay(500);
  });

  nb.onConnected([]() {
    Serial.print("[user] NB-IoT Network connected at (");
    Serial.print(millis());
    Serial.println("ms)");
    Serial.println(nb.createUdpSocket("103.20.205.85", 5683, UDPConfig::ENABLE_RECV));
    Serial.println(nb.createUdpSocket("103.212.181.167", 55566, UDPConfig::ENABLE_RECV));
    flag = 1;
    delay(1000);
  });
  nb.rebootModule();
  // nb.begin();
  // nb.activate();
}
uint32_t prev = millis();

void loop()
{
  nb.loop();
  if (flag && (millis() - prev > 5000)) {
     char buffer[60];
     str2Hex(String(String("{\"millis\": ")+ millis() + String("}")).c_str(), buffer);
     String p3 = "";
     p3 += String("40");
     p3 += String("020579");
     p3 += String("b5");
     p3 += String("4e42496f54"); // NB-IoT
     p3 += String("0d");
     p3 += String("17");
     p3 +=  String(tokenHex);
     p3 += String("ff");
     p3 += String(buffer);

    // if (nb.sendMessageHex(p3.c_str(), 1)) {
    //   Serial.println(">> [cmmc] socket1: send ok.");
    // }
    if (nb.sendMessageHex(p3.c_str(), 0)) {
      Serial.println(">> [ais] socket0: send ok.");
      // 96655121255 -->
      // 504848 ---> 200
    }
    // char buu[100];
    // nb._writeCommand("AT+NSORF=1,100", 10L, buu);
    // Serial.println(buu);
    prev = millis();
  }
}