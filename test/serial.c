#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"

#define PORT "/dev/ttyUSB0"

int main(int argc, char ** argv)
{
	xbee_open((char *)PORT);

	//char * buf =  "\x7E\x00\x04\x08\x01\x4D\x59\x50";

	//char * buf = "\x7E\x00\x10\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\x00\x00\x22\x44\x8A";
	//char * buf = "\x7E\x00\x04\x08\x01\x4A\x4E\x5E";
	//char * buf = "\x7E\x00\x04\x08\x01\x57\x52\x4D";
	//write(xbee->fd, buf, 8); 
	//
	//xbee_send_data("\x22", 1, 0, 0);
	
	//struct at_command * at = malloc(sizeof(struct xbee_atcommand));
	//at->at_cmd = "MY";
	//at->at_cmd = 0x4D59;

	xbee_send_atcommand("JN", "", 0);
	//xbee_send_atwr(xbee->fd, "JN", "\x01", 0);

	struct xbee_rawframe * frame = xbee_read();

	xbee_print_frame((uint8_t *)frame);

	free(frame);
	
	
	/* Closing file descriptor */
	xbee_close();
	return 0;
}
