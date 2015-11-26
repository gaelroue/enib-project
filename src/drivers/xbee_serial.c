#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include <termios.h>
#include <fcntl.h>
#include <poll.h>

static struct xbee_serial * xbee;

static void xbee_open_serial(struct xbee_serial * s)
{
	struct termios tty;

	/* Open device */
	/*
	 * configuration options
	 * 	O_RDWR - we need read and write access
	 *	O_CTTY - prevent other input (like keyboard) from affecting what we read
	 *	O_NDELAY - We don't care if the other side is connected (some devices don't explicitly connect)
	 */
	if ((s->fd = open(s->device, O_RDWR | O_NOCTTY )) < 0) {
		perror(s->device);
		exit(1);
	}

	/* Read parameters */
	tcgetattr(s->fd, &tty);

	/* Change baud rate (output/input) to 9600*/
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);
	
	// Set 8-bit mode
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	tty.c_cc[VMIN]  = 100;
	tty.c_cc[VTIME] = 10;


	// Enable the receiver and set local mode...
	//tty.c_cflag |= (CLOCAL | CREAD);  | ICANON 
	tty.c_lflag &= ~(ECHO | ECHONL | IEXTEN);

	/* Apply new configuration*/
	if (tcsetattr(s->fd, TCSANOW, &tty) == -1) {
		return errno;
	}

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

static int xbee_get_header(int fd, struct xbee_header * h)
{
	uint8_t buf[4];
	int i;
	for(i = 0; i < 4; i++) {
		if (read(fd, &buf[i], 1) < 1) {
			perror("Error getting frame header");
			return;
		}
	}

	if (buf[0] != 0x7E) {
		return -1;
	}
	h->delimiter = buf[0];
	h->length = ((uint16_t)buf[2] << 8) | buf[1];
	h->api = buf[3];
}

static void xbee_get_rawdata(int fd, uint8_t * data, uint16_t length)
{
	if (read(fd, data, length) < 1) {
		perror("Error getting frame data");
		return;
	}
}

static int xbee_read_serial(struct xbee_serial * s, struct xbee_rawframe * frame)
{
	struct pollfd fds;
	int timeout_msecs = 500;
	int ret = 0;

	fds.fd = s->fd;
	fds.events = POLLIN;

	do {
		ret = poll(&fds, 1, timeout_msecs);
	} while (ret == 0);

	if (ret < 0) {
	/* An event on one of the fds has occurred. */
		return;
	}

	if (fds.revents & POLLIN ) {
		if (xbee_get_header(s->fd, &frame->header) == -1) {
			return -1;
		}

		assert(frame);
		assert(frame->header.length);

		/* length to get checksum */
		frame = realloc(frame, sizeof(struct xbee_rawframe) + (frame->header.length) * sizeof(uint8_t));
		xbee_get_rawdata(s->fd, frame->rawdata, frame->header.length);
	}

	return 0;
}

int xbee_read(struct xbee_rawframe * frame)
{
	return xbee_read_serial(xbee, frame);
}

static void xbee_write_serial(int fd, uint8_t * frame)
{
	uint16_t len = ((uint16_t)frame[1] << 8) + (uint16_t)frame[2];

	uint8_t i;
	for ( i = 0; i < 3 + len + 1; i++) {
		write(fd, frame, 1);
		frame++;
	}

}

void xbee_write(uint8_t * frame)
{
	xbee_write_serial(xbee->fd, frame);
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
