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
