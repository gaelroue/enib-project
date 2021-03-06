#ifndef XBEE_CLIENT
#define XBEE_CLIENT

void checksum(uint8_t * frame);
void xbee_send_data(uint8_t * data, int len, uint8_t * dest_mac, uint16_t dest_addr);
void xbee_send_atcommand(uint8_t at_cmd[2], uint8_t * data, uint16_t length);
void xbee_send_atwr(uint8_t at_cmd[2], uint8_t * data, uint16_t length);

#endif
