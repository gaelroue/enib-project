#ifndef BASE
#define BASE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/poll.h>
#include <poll.h>
#include <bits/poll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

/** Typedefs : **/
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;


// DEBUG : 
#define __DEBUG__

// FPGA :
#define __FPGA__
// BB :
//#define __BB__


#define CLEF 666
#define LIMIT_SENSOR 50
#define LIMIT_SEMAPHORE 10

#endif
