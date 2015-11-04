#include "base.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"
#include "drivers/timer.h"
#include "drivers/semaphore.h"


#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
void test_sig(void)
{
	printf(" Test \n");
}
 // Fonction pour permettre de quitter le programme proprement.
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
     
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
    ch = getchar();
 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
 
    if(ch !=EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
    }
int main(int argc, char ** argv)
{

	sensor_init_client();
	init_timer(5,0);
/************* EXEMPLE *******************/
	// On a 2 capteurs sur la carte 
	//struct sensor sensor_board[2];
	//sensor_board.
	//uint8_t * data =  "\x00\x03\xFC\x01\x01\x01\x02\x02\x01\x02\x03\x02\x01\x12\xFF"; // essaie inform
	uint8_t * data =  "\x00\x00\xFE\x01";
	uint8_t len = 4;
	uint8_t ip[2] = {0, 0};
	rawdata_sensor(data, len, ip);

	uint8_t * data_2 =  "\x00\x01\xFC\xF2\x01\x01\x01";
	uint8_t len_2 = 4;
	uint8_t ip_2[2] = {0, 0};
	rawdata_sensor(data_2, len_2, ip_2);
	#ifdef __DEBUG__
		printf("PID : %d \n", getpid());
	#endif
		//signal(1024, test_sig);
	while(!kbhit());
	sensor_close_client();
	remove_semaphore();
	return 0;
}
