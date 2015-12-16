#include "base.h"
#include "zigbee/xbee_struct.h"
#include "drivers/xbee_serial.h"
#include "sensor/sensor_client.h"
#include <pthread.h>

pthread_mutex_t mtx;

void xbee_lock_frame(void)
{
	pthread_mutex_lock(&mtx);
}

void xbee_unlock_frame(void)
{
	pthread_mutex_unlock(&mtx);
}

static void * xbee_frame_parser(struct xbee_rawframe * data)
{
	//We protect the frame while it s copying
	struct xbee_rawframe * tmp = data;

	// test de on copie direct la frame ... 
	struct xbee_rawframe * frame_copy = malloc(3 + tmp->header.length +1);
	memcpy(frame_copy,tmp, 3 + tmp->header.length +1);
	xbee_print_frame((uint8_t *)frame_copy);
	/*
	 * Switch on api type
	 * IMPORTANT : Don't forget to to unlock mutex once you get your frame
	 *
	 */

	switch(tmp->header.api) {
		case 0x95: {
				struct xbee_idframe * frame = malloc(3 + frame_copy->header.length +1);
				frame = (struct xbee_idframe *)frame_copy;
				//memcpy(frame, tmp, 3 + tmp->header.length +1);
				
				// xbee_send_data(START_COMMUNICATION, 3, frame->sender_mac, frame->sender_addr);
			}
		case 0x90: {
				struct xbee_rcv_data * frame = malloc(3 + frame_copy->header.length +1);
				// printf("OK\n");
				frame = (struct xbee_rcv_data *)frame_copy;
				//memcpy(frame, tmp, 3 + tmp->header.length +1);
			
				uint8_t ip[2] = {(frame->dest_addr >> 8)&0xFF, frame->dest_addr&0xFF};
				// ip_change(ip);
				rawdata_sensor(frame->data, frame->header.length - 14, ip);
				
			}
		case 0x8b : {
				// struct tx_status * frame = malloc(0x0b);
				// frame = (struct tx_status *)frame_copy;
				// frame_stat(frame);
		}
		default:
		break;
	}
	// xbee_print_frame((uint8_t *)tmp);
	free(tmp);
}

void xbee_start_server(void)
{
	
	for(;;) {
		print_fifo();
		xbee_send_fifo(1);
		struct xbee_rawframe * frame = xbee_read();
		if(frame->header.delimiter != 0){
			
			// xbee_print_frame((uint8_t *)frame);
			xbee_frame_parser(frame);
			// xbee_send_fifo(1);

		}
		
		// print_fifo();
		
		// printf(" On a reçu une trame la voici : "); 
		// 7e 0 16 90 0 13 a2 0 40 94 46 cd 0 0 1 0 4 fd 0 4 4 0 0 0 0 c9 
		// free(frame);
		//pthread_t th;
		//if (pthread_create(&th, NULL, xbee_frame_parser, frame)) {
		//	printf("Error creating thread\n");
		//	continue;
		//}
	}
}
