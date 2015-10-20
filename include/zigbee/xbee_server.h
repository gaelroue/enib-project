#ifndef XBEE_SERVER
#define XBEE_SERVER

void xbee_start_server(void);
void xbee_frame_parser(struct xbee_rawframe * frame);

#endif
