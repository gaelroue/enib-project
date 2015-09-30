#include "base.h"
#include "drivers/xbee_serial.h"

#define PORT "/dev/ttyUSB0"

int main(int argc, char ** argv)
{
	struct xbee_serial * xbee;
	xbee->device = (char *)PORT;

	xbee_open(xbee);

	char * buf =  "\x7E\x00\x04\x08\x01\x4D\x59\x50";
	write(xbee->fd, buf, 8); 


	/* Closing file descriptor */
	xbee_close(xbee);
	return 0;
}
