#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "base.h"
#include "sensor/sensor_process.h"
#include "sensor/sensor_struct.h"
#include "drivers/timer.h"
#include <pthread.h>



int time_sec, time_usec;
struct itimerval timer;
struct timeval ts_before;
struct timeval ts_now;
int i;
uint8_t refresh_time_null[4] = {0, 0, 0, 0};

void interrupt_timer (int signum)
{
   
   gettimeofday (&ts_now, 0);
	 
	 for(i = 1; i < LIMIT_SENSOR; i++){
	 	struct sensor * tmp_sensor = get_sensor_struct(i);
	 	
	 	if( tmp_sensor->id == 0 || memcmp(tmp_sensor->refresh_time, refresh_time_null, 4) == 0){
	 		continue;
	 	}
	 	#ifdef __DEBUG__
	 		printf("id : %d  \n", get_sensor_struct(i)->id);
	 	#endif
	 	// on a isoler les capteurs périodiques.
		if(tmp_sensor->stat.nb_receive_update == 0 && tmp_sensor->stat.state != CHECK){
			tmp_sensor->stat.state = CHECK;
			int thread;
			ThreadData_t td;
			// On lance un thread qui tentera d'obtenir une réponse...
			td.id = i;// l'index i correspond au numéro du thread
    		thread = pthread_create(&td.th,NULL,beagle_check_sensor,&td); //on crée un nouveau thread 
		}	 	


	 }

 	memcpy(&ts_before, &ts_now, sizeof(struct timeval));
}

void init_timer(int tmp_time_sec,int tmp_time_usec)
{

	time_sec = tmp_time_sec;
	time_usec = tmp_time_usec;

	struct sigaction sa;

	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &interrupt_timer;
	sigaction (SIGALRM, &sa, NULL);
	timer.it_value.tv_sec = 0; /*< temps avant le début des interruptions*/
	timer.it_value.tv_usec = 1;
	timer.it_interval.tv_sec = time_sec; /*< temps entre les interruptions */
	timer.it_interval.tv_usec = time_usec;
	start_timer();
}

void start_timer(void)
{
	setitimer (ITIMER_REAL, &timer, NULL);
}
void stop_timer(void)
{
	setitimer (ITIMER_REAL, NULL, NULL);
}
void set_timer(int tmp_time_sec,int tmp_time_usec)
{
	setitimer (ITIMER_REAL, NULL, NULL);
	timer.it_interval.tv_sec = time_sec; /*< temps entre les interruptions */
	timer.it_interval.tv_usec = time_usec;
	start_timer();
}


