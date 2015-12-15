#include "base.h"
#include "drivers/xbee_serial.h"
#include "sensor/sensor_client.h"
#include "zigbee/xbee_server.h"
#include "drivers/semaphore.h"
#include <pthread.h>



int main(void)
{
	xbee_open("/dev/ttyUSB0");
	init_fifo();
	sensor_init_client();
	xbee_start_server();

}
