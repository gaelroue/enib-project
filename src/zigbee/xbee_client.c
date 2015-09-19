#include "base.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"

void checksum(uint8_t * frame)
{
	uint32_t tmp = 0;
	uint16_t len = ((uint16_t)frame[1] << 8) + (uint16_t)frame[2];

	uint16_t i;
	for (i = 0; i < len; i++) {
		tmp += frame[3+i];
	}

	uint8_t result = (0xFF - ((uint8_t)tmp & 0xff));
	frame[3+len] = result;
}
