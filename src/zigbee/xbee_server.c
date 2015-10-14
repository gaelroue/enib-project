#include "base.h"
#include "zigbee/xbee_struct.h"
#include "drivers/xbee_serial.h"


void xbee_start_server(void)
{
	for(;;) {
		struct xbee_rawframe * frame = xbee_read();

		if (!frame) {
			free(frame);
			continue;
		}

		free(frame);
	}
}
