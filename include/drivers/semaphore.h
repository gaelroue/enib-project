#ifndef SEMAPHORE_H
#define SEMAPHORE_H

struct semaphore{
      int semaphore;
      struct sembuf manipSemaphores;
};

int get_semaphore(int key);
int take_semaphore(int sem);
int give_semaphore(int sem);
int remove_semaphore(int sem);
void init_semaphore(int sem);
#endif
