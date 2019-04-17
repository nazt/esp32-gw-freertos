#include "coap.h"
#include <Arduino.h>
int16_t construct(uint8_t *buffer, CoapPacket &packet, IPAddress ip, int port) {
    uint8_t *p = buffer;
    uint16_t running_delta = 0;
    uint16_t packetSize = 0;

    // make coap packet base header
    *p = 0x01 << 6;
    *p |= (packet.type & 0x03) << 4;
    *p++ |= (packet.tokenlen & 0x0F);
    *p++ = packet.code;
    *p++ = (packet.messageid >> 8);
    *p++ = (packet.messageid & 0xFF);
    p = buffer + COAP_HEADER_SIZE;
    packetSize += 4;

    // make token
    if (packet.token != NULL && packet.tokenlen <= 0x0F) {
        memcpy(p, packet.token, packet.tokenlen);
        p += packet.tokenlen;
        packetSize += packet.tokenlen;
    }

    // make option header
    for (int i = 0; i < packet.optionnum; i++)  {
        uint32_t optdelta;
        uint8_t len, delta;

        if (packetSize + 5 + packet.options[i].length >= BUF_MAX_SIZE) {
            return 0;
        }
        optdelta = packet.options[i].number - running_delta;
        COAP_OPTION_DELTA(optdelta, &delta);
        COAP_OPTION_DELTA((uint32_t)packet.options[i].length, &len);

        *p++ = (0xFF & (delta << 4 | len));
        if (delta == 13) {
            *p++ = (optdelta - 13);
            packetSize++;
        } else if (delta == 14) {
            *p++ = ((optdelta - 269) >> 8);
            *p++ = (0xFF & (optdelta - 269));
            packetSize+=2;
        } if (len == 13) {
            *p++ = (packet.options[i].length - 13);
            packetSize++;
        } else if (len == 14) {
            *p++ = (packet.options[i].length >> 8);
            *p++ = (0xFF & (packet.options[i].length - 269));
            packetSize+=2;
        }

        memcpy(p, packet.options[i].buffer, packet.options[i].length);
        p += packet.options[i].length;
        packetSize += packet.options[i].length + 1;
        running_delta = packet.options[i].number;
    }

    // make payload
    if (packet.payloadlen > 0) {
        if ((packetSize + 1 + packet.payloadlen) >= BUF_MAX_SIZE) {
            return 0;
        }
        *p++ = 0xFF;
        memcpy(p, packet.payload, packet.payloadlen);
        packetSize += 1 + packet.payloadlen;
    } 
    return packetSize;
}

uint16_t generate(uint8_t *buffer, IPAddress ip, int port, char *url, COAP_TYPE type, COAP_METHOD method, uint8_t *token, uint8_t tokenlen, uint8_t *payload, uint16_t payloadlen) {
    // make packet
    CoapPacket packet;

    packet.type = type;
    packet.code = method;
    packet.token = token;
    packet.tokenlen = tokenlen;
    packet.payload = payload;
    packet.payloadlen = payloadlen;
    packet.optionnum = 0;
    packet.messageid = rand();

    // use URI_HOST URI_PATH
    String ipaddress = String(ip[0]) + String(".") + String(ip[1]) + String(".") + String(ip[2]) + String(".") + String(ip[3]); 
    packet.options[packet.optionnum].buffer = (uint8_t *)ipaddress.c_str();
    packet.options[packet.optionnum].length = ipaddress.length();
    packet.options[packet.optionnum].number = COAP_URI_HOST;
    packet.optionnum++;

    // parse url
    int idx = 0;
    for (int i = 0; i < strlen(url); i++) {
        if (url[i] == '/') {
            packet.options[packet.optionnum].buffer = (uint8_t *)(url + idx);
            packet.options[packet.optionnum].length = i - idx;
            packet.options[packet.optionnum].number = COAP_URI_PATH;
            packet.optionnum++;
            idx = i + 1;
        }
    }

    if (idx <= strlen(url)) {
        packet.options[packet.optionnum].buffer = (uint8_t *)(url + idx);
        packet.options[packet.optionnum].length = strlen(url) - idx;
        packet.options[packet.optionnum].number = COAP_URI_PATH;
        packet.optionnum++;
    } 

    uint16_t s = construct(buffer, packet, ip, port); 
    return s;
}
