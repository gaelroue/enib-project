#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"

#define PORT "/dev/ttyUSB0"

int main(int argc, char ** argv)
{
	struct xbee_serial * xbee = malloc(sizeof(struct xbee_serial));
	xbee->device = (char *)PORT;
	struct xbee_rawframe * frame 	;
	xbee_open(xbee);
	while(1){
	frame = xbee_read(xbee);

	xbee_print_rawframe(frame);

	free(frame);
	
	
	/* Closing file descriptor */
	xbee_close(xbee);
	return 0;
}
