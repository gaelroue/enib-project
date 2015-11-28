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

static void * xbee_frame_parser(void * data)
{
	//We protect the frame while it s copying
	pthread_mutex_lock(&mtx);
	struct xbee_rawframe * tmp = (struct xbee_rawframe *)data;

	/*
	 * Switch on api type
	 * IMPORTANT : Don't forget to to unlock mutex once you get your frame
	 */
	switch(tmp->header.api) {
		case 0x95: {
				struct xbee_idframe * frame = malloc(3 + tmp->header.length +1);
				memcpy(frame, tmp, 3 + tmp->header.length +1);
				pthread_mutex_unlock(&mtx);
				uint8_t send_data[3];
				send_data[0] = 0;
				send_data[1] = 0;
				send_data[2] = START_COMMUNICATION;
				xbee_send_data(START_COMMUNICATION, 3, frame->sender_mac, frame->sender_addr);
				free(frame);
			}
		case 0x90: {
				struct xbee_rcv_data * frame = malloc(3 + tmp->header.length +1);
				memcpy(frame, tmp, 3 + tmp->header.length +1);
				pthread_mutex_unlock(&mtx);
				uint8_t ip[2] = {(frame->dest_addr >> 8)&0xFF, frame->dest_addr&0xFF};
				rawdata_sensor(frame->data, frame->header.length - 14, ip);
				free(frame);
			}
		default:
			pthread_mutex_unlock(&mtx);
		break;
	}
}

void xbee_start_server(void)
{
	struct xbee_rawframe * frame = malloc(sizeof(struct xbee_rawframe));
	for(;;) {
		if (xbee_read(frame) == -1) {
			xbee_read_failed(frame);
		}
		xbee_print_frame((uint8_t *)frame);

		pthread_t th;
		if (pthread_create(&th, NULL, xbee_frame_parser, frame)) {
			printf("Error creating thread\n");
			exit(1);
		}
		xbee_frame_parser(frame);


	}
	free(frame);
}
