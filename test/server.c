#include "base.h"
#include "zigbee/xbee_server.h"
#include "sensor/sensor_client.h"
#include <pthread.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("use : ./prg PORT\n");
	}
	xbee_open((char *)argv[1]);

	sensor_init_client();
	xbee_start_server();
}
