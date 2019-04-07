#include "CMMC_RX_Parser.h"

uint8_t START_BYTE_1 = 0x7e;
uint8_t START_BYTE_2 = 0x7f;
uint8_t STOP_BYTE_1 = 0x0d;
uint8_t STOP_BYTE_2 = 0x0a;

void CMMC_RX_Parser::init() {

}

void CMMC_RX_Parser::_parse(uint8_t data) {
  Serial.print(data, HEX);
  switch (_state) {
    case WAIT_STATE :
      if (data == START_BYTE_1) {
        char data_next = -1;
        while ( (data_next = this->_serial->read()) == -1) {
          Serial.println("[1] wait data..");
          delay(1);
        }
        Serial.print(data_next, HEX);
        if (data_next == START_BYTE_2) {
          _state = CMD_STATE;
        }
      }
      break;
    case CMD_STATE :
      _packet.cmd = data;
      _state = DATA_STATE;
      break;
    case DATA_STATE :
      if (data == STOP_BYTE_1) {
        char data_next = -1;
        while ( (data_next = this->_serial->read()) == -1) {
          Serial.println("[2] wait data..");
          delay(1);
        }
        Serial.print(data_next, HEX);
        if (data_next == STOP_BYTE_2) {
          _state = WAIT_STATE;
          memcpy(&_user_packet, &_packet, sizeof(_packet));
          this->_user_on_data(&_user_packet, sizeof(_packet));
          _packet.len = 0;
          break;
        }
        else {
          _packet.data[_packet.len++] = data;
          _packet.data[_packet.len++] = data_next;
        }
      }
      else {
        _packet.data[_packet.len++] = data;
      }
      break;
    default:
      break;
  }
}


void CMMC_RX_Parser::process() {
  while (_serial->available()) {
    _parse(_serial->read());
  }
  yield();
}