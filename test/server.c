#include "base.h"
#include "zigbee/xbee_struct.h"
#include "drivers/xbee_serial.h"
#include <pthread.h>

pthread_mutex_t mtx;

static void * xbee_frame_parser(void * data)
{
	//We protect the frame while it s copying
	printf("in thread\n");
	struct xbee_rawframe * tmp = (struct xbee_rawframe *)data;
	/*
	 * Switch on api type
	 * IMPORTANT : Don't forget to to unlock mutex once you get your frame
	 */
	switch(tmp->header.api) {
		case 0x95: {
				struct xbee_idframe * frame = (struct xbee_idframe *)data;

				xbee_print_frame((uint8_t *)frame);
				uint8_t send_data[3];
				send_data[0] = 0;
				send_data[1] = 0;
				send_data[2] = START_COMMUNICATION;
				xbee_send_data(START_COMMUNICATION, 3, frame->sender_mac, frame->sender_addr);

			}
		case 0x90: {
				struct xbee_rcv_data * frame = (struct xbee_rcv_data *)data;

				xbee_print_frame((uint8_t *)frame);

				uint8_t ip[2] = {(frame->dest_addr >> 8)&0xFF, frame->dest_addr&0xFF};
				rawdata_sensor(frame->data, (uint8_t) htons(frame->header.length) - 12, ip);

			}
		default:
				xbee_print_frame((uint8_t *)tmp);
		break;
	}
	free(data);
	printf("FIN thread\n");
}

void xbee_start_server(void)
{
	sensor_init_client();
	for(;;) {
		struct xbee_rawframe * frame = malloc(sizeof(struct xbee_rawframe));
		printf("DEBUT main\n");
		pthread_mutex_lock(&mtx);
		printf("DEBUT mtx\n");
		if (xbee_read(frame) == -1) {
			continue;
		}
		pthread_mutex_unlock(&mtx);

		printf("DEBUT thread\n");
		pthread_t th;
		if (pthread_create(&th, NULL, xbee_frame_parser, frame)) {
			printf("Error creating thread\n");
			exit(1);
		}


	}
}
