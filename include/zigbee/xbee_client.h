#ifndef XBEE_CLIENT
#define XBEE_CLIENT

void checksum(uint8_t * frame);
int xbee_send_data(int fd, uint8_t * data, uint8_t * dest_mac, uint16_t dest_addr);

#endif
