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

    for (size_t i = 0; i < MAX_ARRAY; i++) {
      pm25_array[i] = 0.0;
      pm10_array[i] = 0.0;
    }
}

void CMMC_DustSensor::_calculateDustAverage() {
  // SERIAL0.printf("pm10=%f\r\n", pm10);
  if (dustIdx < MAX_ARRAY) {
    dust_average25 = median(pm25_array, dustIdx + 1);
    dust_average10 = median(pm10_array, dustIdx + 1);
  }
  else {
    dust_average25 = median(pm25_array, MAX_ARRAY);
    dust_average10 = median(pm10_array, MAX_ARRAY);
  }
  // SERIAL0.printf(">> [avg] %f, %f\r\n", dust_average10, dust_average25);
  // SERIAL0.print(dust_average25);
  // SERIAL0.println(dust_average10);
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
    // SERIAL0.println();
    // SERIAL0.println("---------------------------------------");
    // SERIAL0.println("Concentration Units (standard)");
    // SERIAL0.print("PM 1.0: "); SERIAL0.print(data.pm10_standard);
    // SERIAL0.print("\t\tPM 2.5: "); SERIAL0.print(data.pm25_standard);
    // SERIAL0.print("\t\tPM 10: "); SERIAL0.println(data.pm100_standard);
    // SERIAL0.printf("PM 1.0 = %u<\t\tPM 2.5 = %u<\t\tPM 10=%u<\r\n", data.pm10_standard,
      // data.pm25_standard, data.pm100_standard);
    dustIdx = dust_counter % MAX_ARRAY;
    pm25_array[dustIdx] = data.pm25_standard;
    pm10_array[dustIdx] = data.pm100_standard;
    _calculateDustAverage();
    dust_counter++;
    // SERIAL0.println("---------------------------------------");
    // SERIAL0.println("Concentration Units (environmental)");
    // SERIAL0.print("PM 1.0: "); SERIAL0.print(data.pm10_env);
    // SERIAL0.print("\t\tPM 2.5: "); SERIAL0.print(data.pm25_env);
    // SERIAL0.print("\t\tPM 10: "); SERIAL0.println(data.pm100_env);
    // SERIAL0.println("---------------------------------------");
    // SERIAL0.print("Particles > 0.3um / 0.1L air:"); SERIAL0.println(data.particles_03um);
    // SERIAL0.print("Particles > 0.5um / 0.1L air:"); SERIAL0.println(data.particles_05um);
    // SERIAL0.print("Particles > 1.0um / 0.1L air:"); SERIAL0.println(data.particles_10um);
    // SERIAL0.print("Particles > 2.5um / 0.1L air:"); SERIAL0.println(data.particles_25um);
    // SERIAL0.print("Particles > 5.0um / 0.1L air:"); SERIAL0.println(data.particles_50um);
    // SERIAL0.print("Particles > 10.0 um / 0.1L air:"); SERIAL0.println(data.particles_100um);
    // SERIAL0.println("---------------------------------------");
  }
}
