#include "CMMC_DustSensor.h"
#include "utils.hpp"
// extern HardwareSerial SERIAL0;
CMMC_DustSensor::CMMC_DustSensor(HardwareSerial* s)   {
  this->_serial = s;
}

void CMMC_DustSensor::configLoop() {
  yield();
}

void CMMC_DustSensor::configSetup() {
  yield();
}

float CMMC_DustSensor::getPMValue(dustType_t type) {
  if (type == DustPM10) {
    // return 500+random(millis()) % 499;
    return dust_average10;
  }
  else if (type == DustPM2_5) {
    // return random(millis())%100;
    return dust_average25;
  }
}

void CMMC_DustSensor::setup() {
      // float pm25_array[MAX_ARRAY] = { 0.0 };
      // float pm10_array[MAX_ARRAY] = { 0.0 };
    // SERIAL0.println("initialize DustSensor");
    // SERIAL0.println("CLEARING ARRAY FOR CMMC_DUST");
    String taskMessage = "[CMMC_DustSensor ] Task running on core ";
    taskMessage = taskMessage + xPortGetCoreID();
    // SERIAL0.println(taskMessage);
}

// void CMMC_DustSensor::_calculateDustAverage() {
//   // SERIAL0.printf("pm10=%f\r\n", pm10);
//   if (dustIdx < MAX_ARRAY) {
//     dust_average25 = median(pm25_array, dustIdx + 1);
//     dust_average10 = median(pm10_array, dustIdx + 1);
//   }
//   else {
//     dust_average25 = median(pm25_array, MAX_ARRAY);
//     dust_average10 = median(pm10_array, MAX_ARRAY);
//   }
//   // SERIAL0.printf(">> [avg] %f, %f\r\n", dust_average10, dust_average25);
//   // SERIAL0.print(dust_average25);
//   // SERIAL0.println(dust_average10);
// }
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

void CMMC_DustSensor::loop() {
  this->_serial->begin(9600, SERIAL_8N1, 32 /*rx*/, 33 /* tx */);
  uint32_t ms = millis();

  // vTaskDelay(50 / portTICK_PERIOD_MS);
  // uint8_t command2[] = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };
  // this->_serial->write(command2, sizeof(command2));
  //
  // vTaskDelay(50 / portTICK_PERIOD_MS);
  // uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70 };
  // this->_serial->write(command, sizeof(command));

  vTaskDelay(50 / portTICK_PERIOD_MS);

  while(this->_serial->peek() != 0x42) {
    this->_serial->read();
    if (millis() - ms > 1000) {
      // SERIAL0.println("WAITING.. DUST SENSOR TIMEOUT...");
      break;
    }
  }
  vTaskDelay(200 / portTICK_PERIOD_MS);
  // SERIAL0.println();
  // SERIAL0.println("Reading Dust Sensor..");
  this->readDustSensor();
}

void CMMC_DustSensor::readDustSensor() {
  // Read a byte at a time until we get to the special '0x42' start-byte

  // Now read all 32 bytes
  if (this->_serial->available() < 32) {
    // SERIAL0.println(".......");
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  this->_serial->readBytes(buffer, 32);
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }

  // for (uint8_t i=2; i<32; i++) {
  //   SERIAL0.print("0x"); SERIAL0.print(buffer[i], HEX); SERIAL0.print(", ");
  // }
  // SERIAL0.println();

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    // SERIAL0.println("Checksum failure");
  }
  else {
    dust_average25 = data.pm25_standard;
    dust_average10 = data.pm100_standard;
    // dust_counter++;
  }
}
