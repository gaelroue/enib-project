//----------------------------------------------------------------------------

#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"


#define PORT "/dev/ttyUSB0"

	int
main(int argc,
		char **argv)
{

	xbee_open((char *)PORT);
	uint8_t data[2];
	uint8_t i;
	while(1){
		sleep(3);
		printf("%x ", i);
		data[0] =i++;
		printf("%x\n", i);
		data[1] =i++;
		xbee_send_data(data, 2, 0, 0);
	}
	/* Closing file descriptor */
	xbee_close();
	return 0;
}

//----------------------------------------------------------------------------
