#include "base.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"
#include "drivers/xbee_serial.h"
#include <arpa/inet.h>

void xbee_send_data(uint8_t * data, int len, uint8_t * dest_mac, uint16_t dest_addr)
{
	struct xbee_dataframe * f  = malloc(sizeof(struct xbee_dataframe) + len * sizeof(uint8_t));
	f->header.delimiter = 0x7e;
	f->header.api = 0x10;
	f->frame_id = 0x01;
	uint8_t mac[8] =  {0x00, 0x13, 0xA2,0x00, 0x40, 0x89, 0xEC, 0x0F};
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
		dest_addr = 0xFFFE;
	}

	f->tx.dest_addr = dest_addr;

	f->tx.radius = 0x00;
	f->tx.options = 0x00;

	memcpy(f->tx.data, data, len);

	f->header.length = htons(2 + sizeof(f->tx) + len);

	checksum((uint8_t *) f);

	xbee_write((uint8_t *) f);
	xbee_print_frame((uint8_t *) f);
	free(f);
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
