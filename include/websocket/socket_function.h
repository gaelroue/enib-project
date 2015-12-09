#ifndef SOCKET_FUNCTIONS
#define SOCKET_FUNCTIONS

#include "base.h"
/***************************************************************************
 * socket utils
 ***************************************************************************/
int socket_info(int sock);
int tcp_server_socket(int port);

/***************************************************************************
* Share Memory 
**************************************************************************/

void init_memoire_partage(int key);
void * get_memory_share_p(void);
void let_memory_share(void * attache_memory);
void socket_server_check_sensor(struct pollfd * fds, int nfds);
#endif