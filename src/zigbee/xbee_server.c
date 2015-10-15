#include "base.h"
#include "zigbee/xbee_struct.h"
#include "drivers/xbee_serial.h"


void xbee_start_server(void)
{
	struct xbee_rawframe * frame = malloc(sizeof(struct xbee_rawframe));
	for(;;) {
		if (xbee_read(frame) == -1) {
			continue;
		}

		xbee_print_frame((uint8_t *)frame);
	}
	free(frame);
}
