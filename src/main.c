#include "base.h"
#include <pthread.h>
#include "zigbee/xbee_server.h"
#include "websocket/serveur_socket.h"
#include "drivers/semaphore.h"
#include "camera/fb.h"

int main(int argc, char ** argv)
{

	xbee_open("/dev/ttyUSB0");
	sensor_init_client();
	init_fifo();
	pthread_t th_camera;
	pthread_t th_xbee;
	pthread_t th_serveur_web;

	init_sem();

	init_camera(argc, argv);

		if (pthread_create(&th_xbee, NULL,camera, 0)) {
			printf("Error creating thread for xbee_server\n");
			return -1;
		}


		if (pthread_create(&th_xbee, NULL,xbee_start_server, 0)) {
			printf("Error creating thread for xbee_server\n");
			return -1;
		}
		xbee_send_atcommand("SL", 0, 0);
			if (pthread_create(&th_serveur_web, NULL,socket_server, 0)) {
			printf("Error creating thread for server_web\n");
			return -1;
		}
		//socket_server();
	  if (pthread_join(th_xbee, NULL)) {
	    perror("pthread_join");
	    return EXIT_FAILURE;
    }
    if (pthread_join(th_serveur_web, NULL)) {
	    perror("pthread_join");
	    return EXIT_FAILURE;
    }

	
	return 0;
}
