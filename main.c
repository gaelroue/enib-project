#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "defs.h"



int main(int argc, char ** argv)
{
	uint8_t test[8] = {0x7E,0x00,0x04,0x08,0x01,0x4D,0x59,0x00}; 
	send_uart(12,test);
	//unsigned char * frame = argv[1];
	return 0;
}



int send_uart(int len, uint8_t * frame){
	// pour test : 
	
	chksum(frame);
	return 0;
}

void chksum(uint8_t * frame){
	uint32_t tmp_calc = 0;
	uint16_t len = ((uint16_t)frame[1]<<8) + (uint16_t)frame[2];
	printf("len : %d\n", len);
	uint16_t i = 0;
	for(i=0;i<len;i++){
		tmp_calc += frame[3+i];
	}
	uint8_t result = (0xFF - ((uint8_t)tmp_calc&0xff));
	printf("Checksum : %d\n", result);
	frame[3+len] = result;
	
}