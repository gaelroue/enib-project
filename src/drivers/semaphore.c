
/* Processus ecrivain */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"
#include "base.h"
#include "drivers/semaphore.h"

int semaphores;
struct sembuf manipSemaphores;

int init_semaphore(void)
{
	if ((semaphores=semget(CLEF,1,0600)) == -1)
	{
		if ((semaphores=semget(CLEF,1,IPC_CREAT|0666)) == -1)
  			{
  			#ifdef __DEBUG__
    		printf("Impossible de creer les semaphores");
    		#endif
    		return -1;
  		}
	}
	manipSemaphores.sem_num=0; 
  	manipSemaphores.sem_op=1;

  	semop(semaphores,&manipSemaphores,1);
}


int take_semaphore(void)
{
	manipSemaphores.sem_num=0;
  	manipSemaphores.sem_op=-1;

  	semop(semaphores,&manipSemaphores,1);
  	return 0;
}

int give_semaphore(void)
{
	manipSemaphores.sem_num=0;
    manipSemaphores.sem_op=1;
    semop(semaphores,&manipSemaphores,1);
    return 0;
}

int remove_semaphore(void)
{
	semctl(semaphores,0,IPC_RMID);
}
