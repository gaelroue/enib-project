#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_server.h"
#include "drivers/semaphore.h"
#include <termios.h>
#include <fcntl.h>
#include <poll.h>

static struct xbee_serial * xbee;

static void xbee_open_serial(struct xbee_serial * s)
{
	struct termios newattr;
	struct termios oldattr;

	if((s->fd = open(s->device, O_RDWR | O_NOCTTY)) < 0) {
		perror("Failed to open serial port");
		exit(EXIT_FAILURE);
	} else if(tcgetattr(s->fd, &oldattr) != 0) {
		perror("Failed to get configuration");
		exit(EXIT_FAILURE);
	}
	newattr = oldattr;

	cfsetispeed(&newattr, B115200);
	cfsetospeed(&newattr, B115200);
	// cfsetispeed(&newattr, B9600);
	// cfsetospeed(&newattr, B9600);
	newattr.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
			| INLCR | IGNCR | ICRNL | IXON);
	newattr.c_oflag &= ~OPOST;
	newattr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	newattr.c_cflag &= ~(CSIZE | PARENB | HUPCL);
	newattr.c_cflag |= CS8;
	newattr.c_cc[VMIN]  = 50;
	newattr.c_cc[VTIME] = 1;

	if(tcsetattr(s->fd, TCSANOW, &newattr) != 0) {
		perror("Failed to set configuration");
		exit(EXIT_FAILURE);
	}

	tcflush(s->fd,TCIOFLUSH);
}



void xbee_open(char * port)
{
	xbee = malloc(sizeof(struct xbee_serial));
	xbee->device = port;
	xbee_open_serial(xbee);
}

static void xbee_close_serial(struct xbee_serial * s)
{
	if (close(s->fd) != 0) {
		perror(s->device);
		exit(1);
	}
}

void xbee_close(void)
{
	xbee_close_serial(xbee);
}

static void xbee_get_header(int fd, uint8_t buf[4])
{

	if (read(fd, buf, 4) < 1) {
		perror("Error getting frame header");
		return;
	}
	
}

static void xbee_get_rawdata(int fd, uint8_t * data, uint16_t length)
{
	
	if (read(fd, data, length) < 1) {
		perror("Error getting frame data");
		return;
	}
	
}

static struct xbee_rawframe * xbee_read_serial(struct xbee_serial * s)
{
	struct pollfd fds[1];
	int timeout_msecs = 1000;
	int ret = 0;

	fds[0].fd = s->fd;
	fds[0].events = POLLIN;
// printf(" Avant Poll \n"); 
	// do {
		ret = poll(fds, 1, timeout_msecs);
		// printf(" Après Poll \n"); 
	// } while (ret == 0);
		// printf("ret = %d\n", ret);
	if (ret < 0) {
		printf(" Retour < 0"); 
	/* An event on one of the fds has occurred. */
		struct xbee_rawframe * tmp = (struct xbee_rawframe *)calloc(1,sizeof(struct xbee_rawframe )); 
		return tmp;
	}

	if (fds[0].revents & POLLIN ) {
		uint8_t buf[4];
		while(take_sem_UART() == -1){
				printf("Attente du semaphore\n");
		}
		xbee_get_header(s->fd, buf);
		if (buf[0] != 0x7E) {

			printf("xbee_read_failed : buf = %x %x %x %x  \n", buf[0], buf[1], buf[2], buf[3]);
			give_sem_UART();
			return xbee_read_failed();
		}
		uint16_t length = ((uint16_t)buf[2] << 8) | buf[1];
		
		struct xbee_rawframe * frame = malloc(sizeof(struct xbee_rawframe) + (length) * sizeof(uint8_t));
		frame->header.delimiter = buf[0];
		frame->header.length = length;
		frame->header.api = buf[3];
		
		/* length to get checksum */
		xbee_get_rawdata(s->fd, frame->rawdata, frame->header.length);
		give_sem_UART();
		return frame;
	}

struct xbee_rawframe * tmp = (struct xbee_rawframe *)calloc(1,sizeof(struct xbee_rawframe )); 
		return tmp;
}

struct xbee_rawframe * xbee_read(void)
{
	return xbee_read_serial(xbee);
}

static void xbee_write_serial(int fd, uint8_t * frame)
{
	uint16_t len = ((uint16_t)frame[1] << 8) + (uint16_t)frame[2];

	// uint8_t frame_t[19] = {0x7E, 0x00, 0x0F , 0x10 , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0xFF , 0xFF , 0x00 , 0x00 , 0x00 , 0x00 , 0x52 , 0x9E};
	uint8_t i;
	// 3 + len + 1
	for ( i = 0; i < 3 + len + 1; i++) {
		write(fd, frame, 1);
		frame++;
	}



}

void xbee_write(uint8_t * frame)
{
	while(take_sem_UART() == -1){
				printf("Attente du semaphore\n");
			}
			// printf("Prise du sémaphore UART Write.. ");
	xbee_write_serial(xbee->fd, frame);
	
	give_sem_UART();
	// printf(" Give sémaphore UART Write\n");
}

struct xbee_rawframe * xbee_read_failed(void)
{
	uint8_t buf[4];
	while(buf[0] != 0x7e) {
		if (read(xbee->fd, &buf[0], 1) < 1) {
			perror("Error getting frame header");
			return 0;
		}
	}
	int i;
	for(i = 1; i < 4; i++) {
		if (read(xbee->fd, &buf[i], 1) < 1) {
			perror("Error getting frame header");
			return 0;
		}
	}
	uint16_t length = ((uint16_t)buf[2] << 8) | buf[1];
	struct xbee_rawframe * frame = malloc(sizeof(struct xbee_rawframe) + (length) * sizeof(uint8_t));
	frame->header.delimiter = buf[0];
	frame->header.length = length;
	frame->header.api = buf[3];

	/* length to get checksum */
	xbee_get_rawdata(xbee->fd, frame->rawdata, frame->header.length);
	return frame;
}

void xbee_print_frame(uint8_t * frame)
{
	uint16_t len = ((uint16_t)frame[1] << 8) + (uint16_t)frame[2];

	uint16_t i;
	for (i = 0; i < 3 + len + 1; i++) {
		printf("%x ", frame[i]);
	}
	printf("\n");

}
