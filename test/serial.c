#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"

#define PORT "/dev/ttyUSB0"

int main(int argc, char ** argv)
{
	struct xbee_serial * xbee = malloc(sizeof(struct xbee_serial));
	xbee->device = (char *)PORT;

	xbee_open(xbee);

	char * buf =  "\x7E\x00\x04\x08\x01\x4D\x59\x50";
	write(xbee->fd, buf, 8); 

	struct xbee_rawframe * frame = xbee_read(xbee);

	int i = 0;
	for (i = 0; i < frame->header.length - 1; i++) {
		printf("%x\n", frame->rawdata[i]);
	}

	free(frame);
	
	/* Closing file descriptor */
	xbee_close(xbee);
	return 0;
}
