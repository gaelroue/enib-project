#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#define USECREQ 250
#define LOOPS   1000
int i;
void event_handler (int signum)
{
   struct timeval tsNow;
   gettimeofday (&tsNow, 0);
 i++;
 printf("test : timestamp = %d.%d \n", tsNow.tv_sec, tsNow.tv_usec);
}

int main (int argc, char **argv)
{
 struct sigaction sa;
 struct itimerval timer;

 memset (&sa, 0, sizeof (sa));
 sa.sa_handler = &event_handler;
 sigaction (SIGALRM, &sa, NULL);
 timer.it_value.tv_sec = 1; /*< temps avant le début des interruptions*/
 timer.it_value.tv_usec = USECREQ;
 timer.it_interval.tv_sec = 1; /*< temps entre les interruptions */
 timer.it_interval.tv_usec = 0;
 setitimer (ITIMER_REAL, &timer, NULL);
 while (1);
}
