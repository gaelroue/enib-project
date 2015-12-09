/***** Web Socket server C ****/

#include "base.h"
#include "websocket/serveur_socket.h"

int main(int argc, char ** argv)
{
	init_memoire_partage(CLEF);
	socket_server();
	return 0;
}
