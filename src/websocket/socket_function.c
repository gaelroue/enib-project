#include "base.h"
#include "websocket/socket_function.h"
#include "sensor/sensor_struct.h"
#include "drivers/semaphore.h"
/***************************************************************************
 * socket utils
 ***************************************************************************/
 int socket_info(int sock)
{
	struct sockaddr_in	addr;
	socklen_t			len;
	
	len = sizeof(struct sockaddr_in);
	if (getsockname(sock, (struct sockaddr*)&addr, &len) < 0) {
		perror("getsockname");
		return -1;
	}
	printf("Server IP: %s, port: %u\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	
	return 0;
}

 int tcp_server_socket(int port)
{
	int sock, on=1;
	struct sockaddr_in serv_addr;  /* socket address for server */
	
    /* now create the server socket 
       make it an IPV4 socket (PF_INET) and stream socket (TCP)
       and 0 to select default protocol type */          
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("  cannot open socket");
        return -1;
    }
  
  	/* allow socket descriptor to be reusable */
  	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
  		perror("  setsockopt() failed");
  		close(sock);
  		return -1;
  	}
  	
    /* now fill in values of the server sockaddr_in struct 
       s_addr and sin_port are in Network Byte Order (Big Endian)
       Byte order conversion may be necessary (e.g. htons(), and htonl() */    
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;  /* again ipv4 */  
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* local address */
    serv_addr.sin_port = htons(port); 
        
    /* now bind server port 
       associate server socket (sock) with IP address:port (serv_addr) */ 
    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) {
        perror("  bind() failed");
        close(sock);
        return -1;
    }
    
    /* wait for connection from client with a pending queue of size 5 */
    if (listen(sock, 5) < 0) {
    	perror("  listen() failed");
    	close(sock);
    	return -1;
    }
    
	return sock;
}
/***************************************************************************
* Share Memory 
**************************************************************************/
int memoire_partage_sensor;
int memoire_partage_nb_sensor;
int nb_sensor = 1;
struct sensor * memory_attach;

int semaphore;

void init_memoire_partage(int key, int * memory_share)
{
if ((*memory_share=shmget(key,sizeof(struct sensor)*LIMIT_SENSOR,IPC_CREAT|0666)) == -1)
  {
  	#ifdef __DEBUG__
    	printf("Impossible de creer le segment de memoire partagee");
    #endif 
    exit(1);
  }
  // prise du sémaphore
  semaphore = get_semaphore(key);

}

void * get_memory_share_p(int fd_memoire)
{

  void * attache_memory = shmat(fd_memoire,NULL,0);


  if ((int)(attache_memory)==-1)
  {
    #ifdef __DEBUG__
      printf("Impossible d'attacher ! \n");
    #endif
    exit(1);
  }

}

void let_memory_share(void * attache_memory)
{
  shmdt(attache_memory);
}


void socket_server_check_sensor(struct pollfd * fds, int nfds)
{
  
  nb_sensor = 2; // TO DO : récupérer le nombre de capteur connecter.

  char buf[500]; 

  //strcat(buf, " \"sensor\": { \"nb_sensor\" :[{ \"value\" :]");
  sprintf(buf, "{ \"nb_sensor\" : %d, \
                    \"sensor\" : [ \n", nb_sensor);

  int i;
  char tmp_buf[100];
  for(i = 0; i < nb_sensor; i++){
    int type = 12 +i;
    int value = 5 +i;
    int refresh = 1+i;
    if(i+1 == nb_sensor){
      sprintf(tmp_buf, "{ \"type\" : %d,\n \"value\":%d,\n \"refresh\":%d }", type, value, refresh);
    }else{
      sprintf(tmp_buf, "{ \"type\" : %d, \n \"value\":%d,\n \"refresh\":%d },", type, value, refresh);  
    }
    strcat(buf,tmp_buf);
  }

  
  sprintf(tmp_buf, "\n]} \n");
  strcat(buf,tmp_buf);
  #ifdef __DEBUG__
    printf("buf : %s \n", buf);
  #endif
  for (int k=1;k<nfds;k++) write(fds[k].fd, buf, strlen(buf));
  // puis parcourir les capteurs, former le JSON et l'envoyer au socket.
  // ATTENTION ndfs commence forcément à 1
/*
    strcat(variable, "concatener");

    format JSON :
     
           {"nb_sensor": 1,
            "sensors": [ {"type": 25,
                         "value": 1,
                         "refresh": 2},

                         {"type": 2,
                         "value": 1,
                         "refresh": 3}  
                        ]

}
            

  */

}

