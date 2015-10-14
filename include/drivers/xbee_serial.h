#ifndef XBEE_SERIAL
#define XBEE_SERIAL

struct xbee_serial {
	char * device;
	int fd;		//file descriptor
};

void xbee_open(char * port);
void xbee_close(void);
struct xbee_rawframe * xbee_read(void);
void xbee_write(uint8_t * frame);
void xbee_print_frame(uint8_t * frame);

#endif
