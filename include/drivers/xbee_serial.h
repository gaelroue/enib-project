#ifndef XBEE_SERIAL
#define XBEE_SERIAL

struct xbee_serial {
	char * device;
	int fd;		//file descriptor
};

void xbee_open(struct xbee_serial * s);
void xbee_close(struct xbee_serial * s);
struct xbee_rawframe * xbee_read(struct xbee_serial * s);
void xbee_write(int fd, uint8_t * frame);
void xbee_print_frame(uint8_t * frame);

#endif
