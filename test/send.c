//----------------------------------------------------------------------------

#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"
#include "zigbee/xbee_server.h"
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <poll.h>

void * send_to_xbee(void)
{
uint8_t data[4] = {0x00, 0x01, 0xFE, 0x04};
// 00 01 FE 04
	uint8_t i;
	sleep(1);
	// xbee_send_atcommand("sh", 0, 0);
	xbee_send_atcommand("sl", 0, 0);
	 // while(1){
	 // 	usleep(250000); // 50ms
	 // 	// printf("%x ", i);
	 // 	i++;
	 // 	data[1] = i;
	 // 	xbee_send_data(data,4, 0, 0);
	 // }
	

	 // xbee_send_atcommand("SL",0x00, 0x00);
}

int main(int argc, char **argv)
{
// 	if (argc < 2) {
// 		printf("use : ./prg PORT\n");
	// }
	init_fifo();
	xbee_open("/dev/ttyUSB0");
	pthread_t th;
		if (pthread_create(&th, NULL, send_to_xbee, 0)) {
			printf("Error creating thread\n");
		}
	
	 xbee_start_server();
	/* Closing file descriptor */
		while(1);
	xbee_close();
	return 0;



}

//----------------------------------------------------------------------------
