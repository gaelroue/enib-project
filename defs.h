#ifndef DEFS_H
#define DEFS_H 1

/** Typedefs : **/
typedef unsigned char 		uint8_t;
typedef unsigned short 		uint16_t;
typedef unsigned int		uint32_t;



int send_uart(int len, uint8_t * frame);
void chksum(uint8_t * frame);

#endif