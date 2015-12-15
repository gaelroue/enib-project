#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main(int argc,char** argv)
{
        struct termios tio;
       
        int tty_fd;


        memset(&tio,0,sizeof(tio));
        tio.c_iflag=0;
        tio.c_oflag=0;
        tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
        tio.c_lflag=0;
        tio.c_cc[VMIN]=4;
        tio.c_cc[VTIME]=0;

        tty_fd=open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);      
        cfsetospeed(&tio,B9600);            // 115200 baud
        cfsetispeed(&tio,B9600);            // 115200 baud

        tcsetattr(tty_fd,TCSANOW,&tio);
        char * buf;
        *buf = 0;
      while(1){
      	read(tty_fd, buf, 1);
      	printf(" %x ", buf);
      	usleep(50000);
      }

        close(tty_fd);
        // tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);

        return EXIT_SUCCESS;
}