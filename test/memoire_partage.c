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


#define LONGUEUR_SEGMENT 512 


int main(int argc,char *argv[])
{
  
  int memoirePartagee;
  struct sensor * attacheMoi;

  

  if ((memoirePartagee=shmget(CLEF,sizeof(struct sensor)*LIMIT_SENSOR,IPC_CREAT|0666)) == -1)
  {
    puts("Impossible de creer le segment de memoire partagee");
    exit(1);
  }

  init_semaphore();


  int i = 0;
  while(i < 5){
    take_semaphore();

  puts("Je prends la ressource");fflush(stdout);

  attacheMoi=shmat(memoirePartagee,NULL,0);


  if ((int)(attacheMoi)==-1)
  {
    puts("Impossible d'attacher !");
    exit(1);
  }
  else
  {
    printf("Adresse de l'attachement : %p \n",attacheMoi);
    attacheMoi[0].id = attacheMoi[0].id + 1;
    shmdt(attacheMoi);
    
  give_semaphore();
  }
  sleep(1);
  i++;
  }



 
  /* Suppression des IPC */

  remove_semaphore();
  
  
  
  
  return 0;

}