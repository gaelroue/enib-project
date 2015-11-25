
/* Processus ecrivain */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"
#include "drivers/semaphore.h"
#include "base.h"

struct semaphore semaphores[LIMIT_SEMAPHORE];

int get_semaphore(int key)
{
    int i;
  for(i = 0; i < LIMIT_SEMAPHORE; i++){
      if(semaphores[i].semaphore == 0){
        break;
      }
  }
	if ((semaphores[i].semaphore=semget(key,1,0600)) == -1)
	{
		if ((semaphores[i].semaphore=semget(key,1,IPC_CREAT|0666)) == -1)
  			{
  			#ifdef __DEBUG__
    		printf("Impossible de creer les semaphores");
    		#endif
    		return -1;
  		}
	}
	  semaphores[i].manipSemaphores.sem_num=0; 
  	semaphores[i].manipSemaphores.sem_op=1;

  	while(semop(semaphores[i].semaphore,&(semaphores[i].manipSemaphores),1)!= -1) ;
    return i;
}


int take_semaphore(int sem)
{
	semaphores[sem].manipSemaphores.sem_num=0;
  semaphores[sem].manipSemaphores.sem_op=-1;

  semop(semaphores[sem].semaphore,&(semaphores[sem].manipSemaphores),1);
  return 0;
}

int give_semaphore(int sem)
{
	semaphores[sem].manipSemaphores.sem_num=0;
  semaphores[sem].manipSemaphores.sem_op=1;
  semop(semaphores[sem].semaphore,&(semaphores[sem].manipSemaphores),1);
  return 0;
}

int remove_semaphore(int sem)
{
  semaphores[sem].semaphore = 0;
	semctl(semaphores[sem].semaphore,0,IPC_RMID);
}

void init_semaphore(int sem)
{
  for (int i = 0; i < LIMIT_SEMAPHORE; i++){
      semaphores[i].semaphore = 0;
  }
}