#include "base.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"
#include "drivers/xbee_serial.h"
#include <arpa/inet.h>

int xbee_send_data(int fd, uint8_t * data, uint8_t * dest_mac, uint16_t dest_addr)
{
	int len = strlen((char *)data);

	struct xbee_dataframe * f  = malloc(sizeof(struct xbee_dataframe) + len * sizeof(uint8_t));
	f->header.delimiter = 0x7e;
	f->header.api = 0x10;
	f->header.frame_id = 0x01;

	int i;
	if (!dest_mac) {
		dest_mac = calloc(8, sizeof(uint8_t));
		for( i = 1; i < 3; i++) {
			dest_mac[8-i] = 0xFF;
		}
	}

	for ( i = 0; i < 8; i++) {
		f->tx.dest_mac[i] = dest_mac[i];
	}

	if (!dest_addr) {
		dest_addr = 0xFFFF;
	}

	f->tx.dest_addr = dest_addr;

	f->tx.radius = 0x00;
	f->tx.options = 0x00;

	memcpy(f->tx.data, data, len);

	f->header.length = htons(2 + sizeof(f->tx) + len);

	checksum((uint8_t *) f);

	xbee_write(fd, (uint8_t *) f);

	free(f);
}

void checksum(uint8_t * frame)
{
	uint32_t tmp = 0;
	uint16_t len = ((uint16_t)frame[1] << 8) + (uint16_t)frame[2];

	uint16_t i;
	for (i = 0; i < len; i++) {
		tmp += frame[3+i];
	}

	uint8_t result = (0xFF - ((uint8_t)tmp & 0xFF));
	frame[3 + len] = result;
}
