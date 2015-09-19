#include "base.h"
#include "drivers/xbee_serial.h"
#include <termios.h>
#include <fcntl.h>


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
	if ((s->fd = open(s->device, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
		perror(s->device);
		exit(1);
	}

	// Configure file descriptor to not block on read() if there aren't
 	// any characters available.
	fcntl(s->fd, F_SETFL, FNDELAY);

	/* Read parameters */
	tcgetattr(s->fd, &tty);

	/* Change baud rate (output/input) to 9600*/
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);
	
	// Set 8-bit mode
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

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
