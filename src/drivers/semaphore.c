
/* Processus ecrivain */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "drivers/semaphore.h"
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"
#include "base.h"


int semaphores;
struct sembuf manipSemaphores;

int init_semaphore(void)
{


	if ((semaphores=semget(CLEF,1,0600)) == -1)
	{
      #ifdef __DEBUG__
        printf("Le sémaphore n'existe pas \n");
        #endif
		if ((semaphores=semget(CLEF,1,IPC_CREAT| IPC_EXCL | 0666)) == -1)
  			{
  			#ifdef __DEBUG__
    		printf("Impossible de creer les semaphores");
    		#endif
    		return -1;
  		}

       semctl(semaphores, 0, SETVAL, 1);
	}

 
    manipSemaphores.sem_num=0;

}


int take_semaphore(void)
{
 

  manipSemaphores.sem_op=-1; // On décrémente 
  manipSemaphores.sem_flg = 0; 


  int retour; 
  int count = 200; // 200 ms d'essaie...
  do{
    retour = semop(semaphores,&manipSemaphores,1); 
    count--;
    usleep(1000); // 1ms
  }while(retour != 0 && count > 0);
   
  if (retour == 0){
    printf("On a pris le sémaphore");
    return 1;
  }else {
    return -1;
  }
}

int give_semaphore(void)
{

  manipSemaphores.sem_op= 1; //Pour un V() on incrémente


   int  retour = semop(semaphores,&manipSemaphores,1); 
 printf("On redonne le semaphore ", retour);
 return 0; 
 
}

int remove_semaphore(void)
{
	semctl(semaphores,0,IPC_RMID);
}

void init_semaphore(int sem)
{
  int i;
  for (i = 0; i < LIMIT_SEMAPHORE; i++){
      semaphores[i].semaphore = 0;
  }
}
