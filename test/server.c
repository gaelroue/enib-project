#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"

#define PORT "/dev/ttyUSB1"

int main(int argc, char ** argv)
{
	xbee_open((char *)PORT);

	xbee_start_server();

	/* Closing file descriptor */
	xbee_close();
	return 0;
}
