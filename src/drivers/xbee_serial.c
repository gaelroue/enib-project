#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include <termios.h>
#include <fcntl.h>
#include <poll.h>


void xbee_open(struct xbee_serial * s)
{
	struct termios tty;

	/* Open device */
	/*
	 * configuration options
	 * 	O_RDWR - we need read and write access
	 *	O_CTTY - prevent otheri input (like keyboard) from affecting what we read
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

	tty.c_cc[VMIN]  = 50;
	tty.c_cc[VTIME] = 10;

	// Enable the receiver and set local mode...
	//tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);

	/* Apply new configuration*/
	if (tcsetattr(s->fd, TCSANOW, &tty) == -1) {
		return errno;
	}

}

void xbee_close(struct xbee_serial * s)
{
	if (close(s->fd) != 0) {
		perror(s->device);
		exit(1);
	}
}

static void xbee_get_header(int fd, struct xbee_header * h)
{
	uint8_t buf[3];
	if (read(fd, buf, 3) < 1) {
		perror("Error getting frame header");
		return;
	}

	h->delimiter = buf[0];
	h->length = ((uint16_t)buf[1] << 8) | buf[2];

}

static void xbee_get_dataframe(int fd, uint8_t * data, uint16_t length)
{
	if (read(fd, data, length) < 1) {
		perror("Error getting frame data");
		return;
	}
}

void xbee_read(struct xbee_serial * s)
{
	struct pollfd fds;
	int timeout_msecs = 500;
	int ret = 0;

	fds.fd = s->fd;
	fds.events = POLLIN;

	int i;

	ret = poll(&fds, 1, timeout_msecs);

	if (ret < 0) {
	/* An event on one of the fds has occurred. */
		return;
	}

	if (fds.revents & POLLIN ) {
		struct xbee_header * xbee_h = calloc(1, sizeof(struct xbee_header));
		xbee_get_header(s->fd, xbee_h);

		assert(xbee_h);
		assert(xbee_h->length);

		uint8_t * data = malloc(xbee_h->length * sizeof(uint8_t));
		xbee_get_dataframe(s->fd, data, 7);
	}
}
