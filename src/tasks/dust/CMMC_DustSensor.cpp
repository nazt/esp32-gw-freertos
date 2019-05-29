#include "CMMC_DustSensor.h"
#include "utils.hpp"
// extern HardwareSerial mySerial;
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
    // mySerial.println("initialize DustSensor");
    // mySerial.println("CLEARING ARRAY FOR CMMC_DUST");
    String taskMessage = "[CMMC_DustSensor ] Task running on core ";
    taskMessage = taskMessage + xPortGetCoreID();
    // mySerial.println(taskMessage);

    for (size_t i = 0; i < MAX_ARRAY; i++) {
      pm25_array[i] = 0.0;
      pm10_array[i] = 0.0;
    }
}

void CMMC_DustSensor::_calculateDustAverage() {
  // mySerial.printf("pm10=%f\r\n", pm10);
  if (dustIdx < MAX_ARRAY) {
    dust_average25 = median(pm25_array, dustIdx + 1);
    dust_average10 = median(pm10_array, dustIdx + 1);
  }
  else {
    dust_average25 = median(pm25_array, MAX_ARRAY);
    dust_average10 = median(pm10_array, MAX_ARRAY);
  }
  // mySerial.printf(">> [avg] %f, %f\r\n", dust_average10, dust_average25);
  // mySerial.print(dust_average25);
  // mySerial.println(dust_average10);
}
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

  while(this->_serial->peek() != 0x42) {
    this->_serial->read();
    if (millis() - ms > 3000) {
      // mySerial.println("WAITING.. DUST SENSOR TIMEOUT...");
      break;
    }
  }
  vTaskDelay(200 / portTICK_PERIOD_MS);
  // mySerial.println();
  // mySerial.println("Reading Dust Sensor..");
  this->readDustSensor();
}

void CMMC_DustSensor::readDustSensor() {
  // Read a byte at a time until we get to the special '0x42' start-byte

  // Now read all 32 bytes
  if (this->_serial->available() < 32) {
    // mySerial.println(".......");
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
  //   mySerial.print("0x"); mySerial.print(buffer[i], HEX); mySerial.print(", ");
  // }
  // mySerial.println();

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    // mySerial.println("Checksum failure");
  }
  else {
    // mySerial.println();
    // mySerial.println("---------------------------------------");
    // mySerial.println("Concentration Units (standard)");
    // mySerial.print("PM 1.0: "); mySerial.print(data.pm10_standard);
    // mySerial.print("\t\tPM 2.5: "); mySerial.print(data.pm25_standard);
    // mySerial.print("\t\tPM 10: "); mySerial.println(data.pm100_standard);
    // mySerial.printf("PM 1.0 = %u<\t\tPM 2.5 = %u<\t\tPM 10=%u<\r\n", data.pm10_standard,
      // data.pm25_standard, data.pm100_standard);
    dustIdx = dust_counter % MAX_ARRAY;
    pm25_array[dustIdx] = data.pm25_standard;
    pm10_array[dustIdx] = data.pm100_standard;
    _calculateDustAverage();
    dust_counter++;
    // mySerial.println("---------------------------------------");
    // mySerial.println("Concentration Units (environmental)");
    // mySerial.print("PM 1.0: "); mySerial.print(data.pm10_env);
    // mySerial.print("\t\tPM 2.5: "); mySerial.print(data.pm25_env);
    // mySerial.print("\t\tPM 10: "); mySerial.println(data.pm100_env);
    // mySerial.println("---------------------------------------");
    // mySerial.print("Particles > 0.3um / 0.1L air:"); mySerial.println(data.particles_03um);
    // mySerial.print("Particles > 0.5um / 0.1L air:"); mySerial.println(data.particles_05um);
    // mySerial.print("Particles > 1.0um / 0.1L air:"); mySerial.println(data.particles_10um);
    // mySerial.print("Particles > 2.5um / 0.1L air:"); mySerial.println(data.particles_25um);
    // mySerial.print("Particles > 5.0um / 0.1L air:"); mySerial.println(data.particles_50um);
    // mySerial.print("Particles > 10.0 um / 0.1L air:"); mySerial.println(data.particles_100um);
    // mySerial.println("---------------------------------------");
  }
}
