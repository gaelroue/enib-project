//----------------------------------------------------------------------------

#include "base.h"
#include "zigbee/xbee_client.h"
#include "websocket/serveur_socket.h"



int main(int argc, char **argv)
{

// 	printf("test \n");
// init_fifo();

// // // on insert 2 data :
// 	uint8_t data[5] = { 0x01, 0x02, 0x03, 4, 5};
// 	xbee_insert_fifo(data, 5);
// 	uint8_t i = 0;
// 	for( i = 0; i < 20; i++){
// 		xbee_insert_fifo(data, 5);
// 		xbee_send_fifo(1);
// 	}
// 	xbee_send_fifo(2);
	
	sensor_init_client();
	
	socket_server();
	
}

//----------------------------------------------------------------------------
