#include "base.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"
#include "drivers/xbee_serial.h"
#include <arpa/inet.h>

uint8_t id = 0;
uint8_t * ptr_courant_tr;
uint8_t * ptr_prochaine_tr;
uint8_t buf_fifo[100];

void xbee_send_data(uint8_t * data, int len, uint8_t * dest_mac, uint16_t dest_addr)
{
	struct xbee_dataframe * f  = malloc(sizeof(struct xbee_dataframe) + len * sizeof(uint8_t));
	f->header.delimiter = 0x7e;
	f->header.api = 0x10;
	f->frame_id = data[1];
	uint8_t mac[8] =  {0x00, 0x13, 0xA2,0x00, 0x40, 0x89, 0xEC, 0x0F}; // USB1

	// uint8_t mac[8] =  {0x00, 0x13, 0xA2,0x00, 0x40, 0x89, 0xEC, 0x1B};
	// uint8_t mac[8] =  {0x00, 0x13, 0xA2,0x00, 0x40, 0x94, 0x46, 0xCD}; //  USB0
	int i;
	if (!dest_mac) {
		dest_mac = calloc(8, sizeof(uint8_t));
		for( i = 1; i < 3; i++) {
			dest_mac[8-i] = 0xFF;
		}
	}

	for ( i = 0; i < 8; i++) {
		f->tx.dest_mac[i] = mac[i];
	}

	if (!dest_addr) {
		dest_addr = 0xFEFF;
	}

	f->tx.dest_addr = dest_addr;

	f->tx.radius = 0x00;
	f->tx.options = 0x00;

	memcpy(f->tx.data, data, len);

	f->header.length = htons(2 + sizeof(f->tx) + len);

	checksum((uint8_t *) f);

	xbee_write((uint8_t *) f);
	// xbee_print_frame((uint8_t *) f);
	printf("Free f :");
//	free(f);
	printf("... Free\n");
}

/*
 * If data = 0x00 then len must be specified.
 * Otherwise data is considered as NULL
 */
void xbee_send_atcommand(uint8_t at_cmd[2], uint8_t * data, uint16_t len)
{
	if (len == 0 && data) {
		len = strlen((char *) data);
	}
		
	struct xbee_atcommand * f  = malloc(sizeof(struct xbee_atcommand) + len * sizeof(uint8_t));
	f->header.delimiter = 0x7e;
	f->header.api = 0x08;
	f->frame_id = 0x01;
	
	/* Reverse length it s like doing htons */
	f->at.at_cmd = (uint16_t)at_cmd[1] << 8 | (uint8_t)at_cmd[0];
	memcpy(f->at.data_at_cmd, data, len);

	f->header.length = htons(2 + sizeof(f->at) + len);

	checksum((uint8_t *) f);

	xbee_write((uint8_t *) f);

	free(f);

}

void xbee_send_atwr(uint8_t at_cmd[2], uint8_t * data, uint16_t len)
{
	xbee_send_atcommand(at_cmd, data, len);
	xbee_send_atcommand("WR", "\x01", 0);
}

void checksum(uint8_t * frame)
{
	uint32_t tmp = 0;
	uint16_t len = ((uint16_t)frame[1] << 8) | (uint16_t)frame[2];

	uint16_t i;
	for (i = 0; i < len; i++) {
		tmp += frame[3+i];
	}

	uint8_t result = (0xFF - ((uint8_t)tmp & 0xFF));
	frame[3 + len] = result;
}

void init_fifo(void)
{
	ptr_courant_tr = buf_fifo;
	ptr_prochaine_tr = buf_fifo;
}
void xbee_insert_fifo(uint8_t * data, uint16_t len)
{
	// On prend le sémaphore : 
	while(take_sem_UART() != 0);
	uint8_t i = 0;
	if(ptr_prochaine_tr + len  > buf_fifo +100){
	 		ptr_prochaine_tr = &buf_fifo[0];
	 	} 
	*ptr_prochaine_tr++ = len >> 8;
	*ptr_prochaine_tr++ = len&0xFF;

	// if(ptr_prochaine_tr >= (buf_fifo + 100))
	//  		{
	//  			ptr_prochaine_tr = &buf_fifo[0];
	//  		}

	for (i = 0; i < len; i++){
	
			*ptr_prochaine_tr = data[i];
			ptr_prochaine_tr++;

			if(ptr_prochaine_tr >= (buf_fifo + 100))
	 		{
	 			ptr_prochaine_tr = &buf_fifo[0];
	 		}
		
	}
	 // ptr_prochaine_tr++;
	// printf("buf_fifo + 100 %p\n ptr_prochaine_tr = %p \n", buf_fifo + 100, ptr_prochaine_tr);

	
	 give_sem_UART();

}
uint8_t ip[2] = {0xFF, 0xFE};
void ip_change(uint8_t * ip_t)
{
	if(ip_t[0] != 0x00 || ip_t[1] != 0x00 && ip_t[0] != ip[0] && ip_t[1] != ip[1]){
		printf("Nouvelle ip : %x %x\n",ip_t[0] ,ip_t[1]);
		// ip[0] = ip_t[1];
		// ip[1] = ip_t[0];
	}
}

void xbee_send_fifo(uint8_t nb_tr)
{
	uint8_t j;
	uint8_t i;
	for(j = 0; j < nb_tr; j++){

		uint8_t tmp_len_1, tmp_len_2;
		tmp_len_1 = ptr_courant_tr[0];
		// *ptr_courant_tr++ = 0x00;
		tmp_len_2 = ptr_courant_tr[1];
		// *ptr_courant_tr++ = 0x00;

		uint16_t len = tmp_len_1 << 8 | tmp_len_2;
		if(len != 0){
		*ptr_courant_tr++ = 0x00;
		*ptr_courant_tr++ = 0x00;
		
		xbee_send_data(ptr_courant_tr, len, 0, 0);
		
		for (i = 0; i < len; ++i)
		{
			ptr_courant_tr[i] = 0x00; 
		}
		// printf("avant len = %p \n", ptr_courant_tr);
		ptr_courant_tr += len;
		// printf("after len = %p \n", ptr_courant_tr);
		 
		 if(ptr_courant_tr >= buf_fifo + 100)
		 {
		 	ptr_courant_tr = &buf_fifo[0];
		 }
		}else{
			if(ptr_courant_tr+2 >= buf_fifo + 100){
				ptr_courant_tr = &buf_fifo[0];		
			}
		}

	}

}

void print_fifo(void)
{
	uint8_t i;
	for(i = 0; i < 100; i++)
	{
		printf("%x ", buf_fifo[i]);
	}
	printf("\n");
}