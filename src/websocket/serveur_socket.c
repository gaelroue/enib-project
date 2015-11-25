/*** Gestion des WebSockets *
* Objectif : attendre une connexion sur une socket 
	- une fois connecté envoyer et recevoir les informations aux sockets.

*/

#include "base.h"
#include "websocket/socket_function.h"
#include "websocket/serveur_socket.h"

int listener;


void start_socket_server(void)
{
	#ifdef __DEBUG__
			printf("Start server... ");
		#endif
	listener = tcp_server_socket(SERVEUR_PORT);
	if (listener < 0 ){
		#ifdef __DEBUG__
			printf("fail. ");
		#endif
		 exit(1);
	}
	#ifdef __DEBUG__
			printf("OK.\n", listener);
		#endif
}

void stop_socket_server(void)
{
	#ifdef __DEBUG__
			printf("Stop server... ");
	#endif
	close(listener);
	#ifdef __DEBUG__
			printf("OK ");
	#endif
}

void socket_server(void)
{
	start_socket_server();
	socket_info(listener);
	struct pollfd fds[10];
    int nfds=0;
	/* initialze the fds to poll */
	memset(fds, 0, sizeof(fds));
	
	fds[0].fd = listener;
	fds[0].events = POLLIN;
	nfds++;

	char buf[MAX_MSG];					// buffer for client data
    int nbytes;

	int timeout=250;	// timeout in ms
	// TO DO : voir si des capteurs sont déjà relier ;)
	char quit = 0;
	while(!quit){

		int rc = poll(fds, nfds, timeout);
		if (rc < 0) {
			#ifdef __DEBUG__
			perror("  poll() failed");
			#endif
			break;
		}	

		if(rc == 0){ // Timeout, will not happen here
			// TO DO : Update the data capteur ;)
			socket_server_check_sensor(fds, nfds);

		}	
		if (fds[0].revents == POLLIN) {
			int 				client;			// client socket descriptor
    		struct sockaddr_in 	clientaddr;		// client address
    		socklen_t 			addrlen;		// used to store length (size) of sockaddr_in
			
			addrlen = sizeof(clientaddr);
			client = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);
			if (client < 0) {
				#ifdef __DEBUG__
				perror("  accept() failed");
				#endif
				break;
			} else {
				#ifdef __DEBUG__
					printf("new connection from %s, port %d on socket %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), client);
				#endif
				fds[nfds].fd = client;
				fds[nfds].events = POLLIN;
				nfds++;
			}
			rc--;
		}

		/* check for data from a connected client */

		int i =1;
		while (i<nfds) {
			if (!rc) break;
			if (fds[i].revents == POLLIN) {
				nbytes=read(fds[i].fd, buf, sizeof(buf));
				if (nbytes < 0) {
					#ifdef __DEBUG__
						perror("read client socket failed!");
					#endif
				} else if (nbytes == 0) { 	// connection closed!
					#ifdef __DEBUG__
						printf("connection on socket %d closed\n", fds[i].fd);
					#endif
					close(fds[i].fd);
					fds[i].fd = -1;
					/* compress the fds array */
					int j;
					for (j=i ; j<nfds-1 ; j++) fds[j].fd = fds[j+1].fd;
					nfds--;
					i--;
				} else {	
					#ifdef __DEBUG__
						printf("socket %d --> %s\n",fds[i].fd, buf);
					#endif
						// TO DO : if user send command -> inform sensor
					}
				
				rc--;
			}
			i++;
		}


	}

	stop_socket_server();
	exit(0);

}
