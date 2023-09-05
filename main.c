#include "stm32f4xx.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "flash.h"
#include "key.h"
#include "tim.h"
#include "ymodem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @bieaf 进行BootLoader的启动
 *
 * @param  none
 * @return none
 */
void bootloader_start(void)
{
    /*==========打印消息==========*/

    switch(read_start_mode())										//读取是否启动应用程序
    {
		case STARTUP_NORMAL:										//正常启动
		{
			printf("> normal start......\r\n");
			break;
		}
		case STARTUP_UPDATE:										//升级再启动
		{
			printf("> start update......\r\n");

			move_code(APPLICATION_1_ADDR,APPLICATION_2_ADDR,APPLICATION_2_SIZE);

			printf("> update success......\r\n");
			break;
		}
		case STARTUP_RESET:											//恢复出厂设置 目前没使用
		{
			printf("> restore to factory program......\r\n");
			break;
		}
		default:													//启动失败
		{
			printf("> error:%X!!!......\r\n", read_start_mode());
			return;
		}
    }

    /* 跳转到应用程序 */
    printf("> start up......\r\n\r\n");

    iap_execute_app(APPLICATION_1_ADDR);
}

//主函数
int main(void)
{
	//led初始化
	led_init();
	
	//beep初始化
	beep_init();	
	
	//按键检测
	key_init();
	
	//定时器3初始化
	tim3_init();
	
	//串口1初始化波特率为115200bps
	usart1_init(115200);
	
	//串口延迟一会，确保芯片内部完成全部初始化,printf无乱码输出
	delay_ms(1000);
	
	printf("This is bootloader test by teacher.wen\r\n");

	//启动后，没有任何按键按下，则调用bootloader_start函数
	if(key_sta_get() == 0)
		bootloader_start();

	printf("now start ymodem download\r\n");
	
    while(1)
    {
		ymodem_download();
    }


    return 0;
}



