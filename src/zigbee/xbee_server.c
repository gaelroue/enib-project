#include "base.h"
#include "zigbee/xbee_struct.h"
#include "drivers/xbee_serial.h"
#include <pthread.h>

pthread_mutex_t mtx;

static void * xbee_frame_parser(void * data)
{
	//We protect the frame while it s copying
	pthread_mutex_lock(&mtx);
	struct xbee_rawframe * tmp = (struct xbee_rawframe *)data;
	struct xbee_rawframe * frame = malloc(3 + tmp->header.length +1);
	memcpy(frame, tmp, 3 + tmp->header.length +1);
	pthread_mutex_unlock(&mtx);

	sleep(4);
	xbee_print_frame((uint8_t *)frame);

	switch(frame->header.api) {
		case 0x90:
			printf("data\n");
		break;
		case 0x95:
			printf("connexion\n");
		break;
		default:
		break;
	}
}

void xbee_start_server(void)
{
	struct xbee_rawframe * frame = malloc(sizeof(struct xbee_rawframe));
	for(;;) {
		if (xbee_read(frame) == -1) {
			continue;
		}

		pthread_t th;
		if (pthread_create(&th, NULL, xbee_frame_parser, frame)) {
			printf("Error creating thread\n");
			exit(1);
		}


	}
	free(frame);
}
