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
 * @bieaf CRC-16 校验
 *
 * @param addr 开始地址
 * @param num   长度
 * @param num   CRC
 * @return crc  返回CRC的值
 */
#define POLY        0x1021  
uint16_t crc16(uint8_t *addr, int32_t num, uint16_t crc)  
{  
    int32_t i;  
    for (; num > 0; num--)					/* Step through bytes in memory */  
    {  
        crc = crc ^ (*addr++ << 8);			/* Fetch byte from memory, XOR into CRC top byte*/  
        for (i = 0; i < 8; i++)				/* Prepare to rotate 8 bits */  
        {
            if (crc & 0x8000)				/* b15 is set... */  
                crc = (crc << 1) ^ POLY;  	/* rotate and XOR with polynomic */  
            else                          	/* b15 is clear... */  
                crc <<= 1;					/* just rotate */  
        }									/* Loop for 8 bits */  
        crc &= 0xFFFF;						/* Ensure CRC remains 16-bit value */  
    }										/* Loop until num=0 */  
    return(crc);							/* Return updated CRC */  
}



/* 设置升级的步骤 */
static enum UPDATE_STATE update_state = TO_START;

void ymodem_set_state(enum UPDATE_STATE state)
{
	update_state = state;
}


/* 查询升级的步骤 */
uint8_t ymodem_get_state(void)
{
	return update_state;
}




/* 发送指令 */
void ymodem_send_cmd(uint8_t command)
{

	USART_SendData(USART1,command);

	//等待数据发送成功
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	delay_ms(10);
}

/* 标记升级完成 */
void update_set_down(void)
{
	uint32_t update_flag = 0xAAAAAAAA;				// 对应bootloader的启动步骤
	
	flash_program((APPLICATION_2_ADDR + APPLICATION_2_SIZE - 4), &update_flag,1 );
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

static 
	uint8_t data_state = 0;

	if(ymodem_get_state()==TO_START)
	{
		ymodem_send_cmd(CCC);
		
		delay_ms(1000);
	}
	
	/* 串口1接收完一个数据包 */
	if(g_usart1_rx_end)    	
	{

		
		/* 清空接收完成标志位、接收计数值 */
		g_usart1_rx_end=0;
		g_usart1_rx_cnt=0;
		
		switch(g_usart1_rx_buf[0])
		{
			case SOH://数据包开始
			{
					crc = 0;
				
					/* 计算crc16 */
					crc = crc16((uint8_t *)&g_usart1_rx_buf[3], 128, crc);
						
					if(crc != (g_usart1_rx_buf[131]<<8|g_usart1_rx_buf[132]))
						return;
					
					if((ymodem_get_state()==TO_START)&&(g_usart1_rx_buf[1] == 0x00)&&(g_usart1_rx_buf[2] == (uint8_t)(~g_usart1_rx_buf[1])))// 开始
					{

						ymodem_set_state(TO_RECEIVE_DATA);
						
						/* 若ymodem_send_cmd执行在sector_erase之前，则导致串口数据丢包，因为擦除会关闭所有中断 */
						/* 擦除应用程序2的扇区 */
						sector_erase(APPLICATION_2_SECTOR);						
						
						data_state = 0x01;						
						ymodem_send_cmd(ACK);
						ymodem_send_cmd(CCC);

	
					}
					else if((ymodem_get_state()==TO_RECEIVE_END)&&(g_usart1_rx_buf[1] == 0x00)&&(g_usart1_rx_buf[2] == (uint8_t)(~g_usart1_rx_buf[1])))// 结束
					{
						update_set_down();						
						ymodem_set_state(TO_START);
						ymodem_send_cmd(ACK);
						
						/* 嘀一声示，表示下载完成 */
						beep_on();delay_ms(80);beep_off();
						
						/* 复位 */
						NVIC_SystemReset();
					}					
					else if((ymodem_get_state()==TO_RECEIVE_DATA)&&(g_usart1_rx_buf[1] == data_state)&&(g_usart1_rx_buf[2] == (uint8_t)(~g_usart1_rx_buf[1])))// 接收数据
					{

						/* 烧录程序 */
						flash_program((APPLICATION_2_ADDR + (data_state-1) * 128), (uint32_t *)(&g_usart1_rx_buf[3]), 32);
						data_state++;
						
						ymodem_send_cmd(ACK);		
					}
			}break;
			
			case EOT://数据包传输结束
			{
				if(ymodem_get_state()==TO_RECEIVE_DATA)
				{

					ymodem_set_state(TO_RECEIVE_EOT2);					
					ymodem_send_cmd(NACK);
				}
				else if(ymodem_get_state()==TO_RECEIVE_EOT2)
				{
	
					
					ymodem_set_state(TO_RECEIVE_END);					
					ymodem_send_cmd(ACK);
					ymodem_send_cmd(CCC);
				}
	
			}break;	
			
			default:break;
		}

	}
}



