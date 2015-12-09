//----------------------------------------------------------------------------

#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"



int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("use : ./prg PORT\n");
	}
	xbee_open((char *)argv[1]);

	uint8_t data[] = { 0x00, 0x01, 0x02};
	xbee_send_data(data, 3, 0, 0);
	/* Closing file descriptor */
	xbee_close();
	return 0;
}

//----------------------------------------------------------------------------
