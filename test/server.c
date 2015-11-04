#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"


int main(int argc, char ** argv)
{
	if (argc < 2) {
		printf("use : ./prg PORT\n");
	}
	xbee_open((char *)argv[1]);

	xbee_start_server();

	/* Closing file descriptor */
	xbee_close();
	return 0;
}
