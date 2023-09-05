#ifndef __YMODEM_H__
#define __YMODEM_H__



#define SOH		0x01
#define STX		0x02
#define ACK		0x06
#define NACK	0x15
#define EOT		0x04
#define CCC		0x43



/* Éý¼¶µÄ²½Öè */
enum UPDATE_STATE
{
	TO_START = 0x01,
	TO_RECEIVE_DATA = 0x02,
	TO_RECEIVE_EOT1 = 0x03,
	TO_RECEIVE_EOT2 = 0x04,
	TO_RECEIVE_END = 0x05
};



extern void ymodem_download(void);



#endif

