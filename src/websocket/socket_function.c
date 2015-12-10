#include "base.h"
#include "websocket/socket_function.h"
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"
#include "drivers/semaphore.h"
#include <math.h>
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
struct sensor * memory_attach;
void * attache_memory;
int semaphore;

void init_memoire_partage(int key)
{
if ((memoire_partage_sensor=shmget(key,sizeof(struct sensor)*LIMIT_SENSOR,IPC_CREAT|0666)) == -1)
  {
  	#ifdef __DEBUG__
    	printf("Impossible de creer le segment de memoire partagee");
    #endif 
    exit(1);
  }
  // prise du sémaphore
  semaphore = init_semaphore();

}

void * get_memory_share_p(void)
{

  attache_memory = shmat(memoire_partage_sensor,NULL,0);


  if ((int)(attache_memory)==-1)
  {
    #ifdef __DEBUG__
      printf("Impossible d'attacher ! \n");
    #endif
    exit(1);
  }
    #ifdef __DEBUG__
         printf("Adresse de l'attachement : %p \n",attache_memory);    
    #endif
 
  return attache_memory;
}

void let_memory_share(void * attache_memory)
{
  shmdt(attache_memory);
}


void socket_server_check_sensor(struct pollfd * fds, int nfds)
{
	srand(time(NULL));
  
 int nb_sensor = 0; // TO DO : récupérer le nombre de capteur connecter.
  // On appel la mémoire :
  // On va lire la mémoire donc sémaphore take :
  // if(!take_semaphore()){
  //   #ifdef __DEBUG__
  //     printf(" Impossible de prendre le semaphore ...\n");
  //   #endif
  //     return;
  // }
  memory_attach = (struct sensor *)get_memory_share_p();
  
  // On parcours tout les sensor, si l'ID est à 0 on passe, sinon on créer le JSOn adéquate.
  char * buf_sensor = (char *)calloc(500, sizeof(char));
  char * buf_to_send = (char *)calloc(500, sizeof(char));

  char * tmp_buf = (char *)calloc(100, sizeof(char));
  sprintf(buf_to_send, "{ \"sensor\" : [");

  int i;
  for( i = 0; i < LIMIT_SENSOR; i++){

    if(memory_attach[i].id == 0) continue;
    if(nb_sensor != 0){
       sprintf(tmp_buf, ",");  
      strcat(buf_sensor,tmp_buf);
    }
    nb_sensor++; // On incrémente le nombre de capteur puisque l'on en ajoute 1.
    int type = memory_attach[i].type;
    int data = memory_attach[i].data[0];
    int data2 = 0;
    int refresh = memory_attach[i].refresh_time[0]<<24 | memory_attach[i].refresh_time[1]<<16 | memory_attach[i].refresh_time[2]<<8 | memory_attach[i].refresh_time[3];
    int id = memory_attach[i].id;
    int exposant = 0;
    int mantisse = 0;
    float value = 0;
    switch(type){
      case SENSOR_TEMP: // Température : 
         value = (float)data/powf(2,8);
        //printf("Valeur = %f\n", value);
          sprintf(tmp_buf, "{ \"id\" : %d,\"type\" : %d, \"value\": %f, \"refresh\": %d }", id, type, value, refresh);  
          strcat(buf_sensor,tmp_buf);
      break;
      case SENSOR_LUMI:
      //printf("\ndata lumi = %x \n", data);
          exposant  = (data&0xF000) >> 12;
          // printf("exposant= %x \n", exposant);
          mantisse  = data&0x0F00 >> 4;
          mantisse |= data&0x000F;
          // printf("mantisse = %x \n", mantisse);
          value = powf(2,exposant)* mantisse *0.045;
          sprintf(tmp_buf, "{ \"id\" : %d,\"type\" : %d, \n \"value\":%f,\n \"refresh\":%d }", id, type, value, refresh);    
          strcat(buf_sensor,tmp_buf);
      break;
      case SENSOR_AXIS:
  
        sprintf(tmp_buf, "{ \"id\" : %d,\"type\" : %d, \n \"value\":[%d,%d,%d],\n \"refresh\":%d }", id, type, (int)memory_attach[i].data[0],(char)memory_attach[i].data[1],(char)memory_attach[i].data[2], refresh);
        strcat(buf_sensor,tmp_buf);
      break;
      case SENSOR_ADC :
        data = (int)100*data/4096;
        data2 = (int)100*memory_attach[i].data[1]/4096;
        sprintf(tmp_buf, "{ \"id\" : %d,\"type\" : %d, \n \"value\": [%d, %d],\n \"refresh\":%d }", id, type, data, data2, refresh);  
        strcat(buf_sensor,tmp_buf);
     break;
      default:
      break;
    }
    
     
  }

  // give_semaphore();

  strcat(buf_to_send,buf_sensor);
  // On close le JSON
  sprintf(tmp_buf, "\n]} \n");
  strcat(buf_to_send,tmp_buf);


  #ifdef __DEBUG__
    printf("buf : %s \n", buf_to_send);
  #endif
  if (nb_sensor != 0) {
    int k;
  for (k=1;k<nfds;k++) write(fds[k].fd, buf_to_send, strlen(buf_to_send));
  }
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
  // give_semaphore();
  return;
}

