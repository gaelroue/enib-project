
/* Processus ecrivain */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "drivers/semaphore.h"
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"
#include "base.h"



pthread_mutex_t sensor_tab_mutex;
pthread_mutex_t UART_mutex;


int init_sem(void)
{


	// if ((semaphores=semget(CLEF,1,0600)) == -1)
	// {
 //      #ifdef __DEBUG__
 //        printf("Le sémaphore n'existe pas \n");
 //        #endif
	// 	if ((semaphores=semget(CLEF,1,IPC_CREAT| IPC_EXCL | 0666)) == -1)
 //  			{
 //  			#ifdef __DEBUG__
 //    		printf("Impossible de creer les semaphores");
 //    		#endif
 //    		return -1;
 //  		}

 //       semctl(semaphores, 0, SETVAL, 1);
	// }

 
 //    manipSemaphores.sem_num=0;

  if (pthread_mutex_init(&sensor_tab_mutex, NULL) != 0)
    {
        printf("\n Sensor mutex init failed\n");
        return 1;
    }
      if (pthread_mutex_init(&UART_mutex, NULL) != 0)
    {
        printf("\n Uart mutex init failed\n");
        return 1;
    }
}


int take_sem_sensor(void)
{

  return pthread_mutex_lock(&sensor_tab_mutex);

  
}

int give_sem_sensor(void)
{

 return pthread_mutex_unlock(&sensor_tab_mutex);
}

int take_sem_UART(void)
{
  int retour = pthread_mutex_lock(&UART_mutex);
  // printf("retour take sem UART = %d", retour);
  return retour;
}

int give_sem_UART(void)
{
int retour = pthread_mutex_unlock(&UART_mutex);
  // printf("retour give sem UART = %d", retour);
 return pthread_mutex_unlock(&UART_mutex);
}

