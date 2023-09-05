#include "stm32f4xx.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "flash.h"
#include "key.h"
#include "ymodem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**
 * @bieaf CRC-16 校验  检测数据传输过程中是否出现错误（某些位，或某几位，或者某块区域位错误）。
 *
 * @param addr 校验数据开始地址
 * @param num   数据长度
 * @param crc   初始的CRC值
 * @return crc  返回CRC的值
 */
#define POLY 			0x1021   //定义了多项式的值，即CRC-16-CCITT所使用的多项式。
uint16_t crc16(uint8_t *addr,int32_t num, uint16_t crc)
{
	int32_t i; 
	//第一步使用循环遍历待校验的数据，迭代次数为数据的长度num
	for(;num > 0;num--)
	{
		//第二步在每次迭代中，通过*addr++取出数据的一个字节，并将指针addr自增，指向下一个字节。
		
		crc = crc ^ (*addr++ << 8); //先取addr指向的字节值，自增指向下一个字节\
		左移8位,然后与CRC值进行异或操作。这样做的目的是确保每个字节都能对CRC值的每个字节都进行处理。CRC算法通过按位异或和位移操作来生成校验值，\
		同时也需要处理到每个字节的位上，并相应地对这些位进行异或操作。
		
		//第三步检查CRC值最高位是否为1
		for(i = 0;i < 8; i++)
		{
			if(crc & 0x8000)//如果是，则将CRC值左移1位并与多项式进行异或操作；
				crc = (crc << 1) ^ POLY;
			else //如果最高位为0，则只将CRC值左移1位。
				crc <<= 1;
		}
		//保证CRC值保持为16位，通过按位与操作将CRC值截断为16位。
		crc &= 0xFFFF;
	}
	return(crc);
}

/* 设置升级的步骤 */
static enum UPDATE_STATE update_state = TO_START; //用来记录升级的当前步骤

void ymodem_set_state(enum UPDATE_STATE state) 
{
	update_state = state;
}


/* 查询升级的步骤 */
uint8_t ymodem_get_state(void)
{
	return update_state;
}

/* ymodem发送指令 */
void ymodem_send_cmd(uint8_t command)
{
	USART_SendData(USART1,command); //通过串口1发送命令
	//等待数据发送成功
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	delay_ms(10);
}

/* 标记升级完成 */
void update_set_down(void)
{
	//这是一个用于标记升级完成的特定值。
	uint32_t update_flag = 0xAAAAAAAA;				// 对应bootloader的启动步骤 
	
	flash_program((APPLICATION_2_ADDR + APPLICATION_2_SIZE - 4), &update_flag,1 );//减去4是为了将update_flag存储在区域末尾的位置。
	
}

/**
 * @bieaf ymodem下载
 *
 * @param none
 * @return none
 */
void ymodem_download(void)
{
	uint16_t crc = 0;
	
}
