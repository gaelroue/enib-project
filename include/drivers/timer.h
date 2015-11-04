#ifndef TIMER_H
#define TIMER_H



void interrupt_timer (int signum);
void init_timer(int tmp_time_sec,int tmp_time_usec);
void start_timer(void);
void stop_timer(void);
void set_timer(int tmp_time_sec,int tmp_time_usec);

#endif

