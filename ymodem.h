#ifndef __YMODEM_H__
#define __YMODEM_H__


//AsCll校验码 
#define SOH		0x01
#define STX		0x02
#define ACK		0x06
#define NACK	0x15
#define EOT		0x04
#define CCC		0x43



/* 升级的步骤 

枚举值可以用于在代码中表示和判断当前升级所处的状态，从而执行相应的逻辑操作。

*/
enum UPDATE_STATE
{
	TO_START = 0x01, //表示升级开始的步骤。
	TO_RECEIVE_DATA = 0x02, //表示接收数据的步骤。
	TO_RECEIVE_EOT1 = 0x03, //表示接收结束标志1的步骤。
	TO_RECEIVE_EOT2 = 0x04, //表示接收结束标志2的步骤。
	TO_RECEIVE_END = 0x05   //表示接收结束的步骤。
};



extern void ymodem_download(void);



#endif

