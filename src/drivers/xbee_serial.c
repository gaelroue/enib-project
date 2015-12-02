#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_server.h"
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

	cfsetispeed(&newattr, B9600);
	cfsetospeed(&newattr, B9600);
	newattr.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
			| INLCR | IGNCR | ICRNL | IXON);
	newattr.c_oflag &= ~OPOST;
	newattr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	newattr.c_cflag &= ~(CSIZE | PARENB | HUPCL);
	newattr.c_cflag |= CS8;
	newattr.c_cc[VMIN]  = 50;
	newattr.c_cc[VTIME] = 10;

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
		uint8_t buf[4];
		xbee_get_header(s->fd, buf);
		if (buf[0] != 0x7E) {
			return xbee_read_failed();
		}
		uint16_t length = ((uint16_t)buf[2] << 8) | buf[1];
		struct xbee_rawframe * frame = malloc(sizeof(struct xbee_rawframe) + (length) * sizeof(uint8_t));
		frame->header.delimiter = buf[0];
		frame->header.length = length;
		frame->header.api = buf[3];

		/* length to get checksum */
		xbee_get_rawdata(s->fd, frame->rawdata, frame->header.length);
		return frame;
	}
}

struct xbee_rawframe * xbee_read(void)
{
	return xbee_read_serial(xbee);
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

struct xbee_rawframe * xbee_read_failed(void)
{
	uint8_t buf[4];
	while(buf[0] != 0x7e) {
		if (read(xbee->fd, &buf[0], 1) < 1) {
			perror("Error getting frame header");
			return;
		}
	}
	int i;
	for(i = 1; i < 4; i++) {
		if (read(xbee->fd, &buf[i], 1) < 1) {
			perror("Error getting frame header");
			return;
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
