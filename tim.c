#include "stm32f4xx.h"
#include "sys.h"
#include "usart.h"

void tim3_init(void)
{
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	NVIC_InitTypeDef   		NVIC_InitStructure;
	//打开TIM3硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//TIM3的硬件时钟频率=84MHz/(TIM_Prescaler+1)=84MHz/(8399+1)=10000Hz
	TIM_TimeBaseStructure.TIM_Period = 9;//计数值，用于定时时间的设置，0 ->9共10次计数
	TIM_TimeBaseStructure.TIM_Prescaler = 8399;//预分频值的配置
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//F407没有，不需要配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数，就是0 -> TIM_Period，然后就会触发时间更新中断
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	/* 定时器时间更新中断使能 */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	//配置NVIC，用于打开TIM3的中断请求通道
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	//TIM3的请求通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级0xF
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//响应优先级0xF
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//打开其请求通道
	NVIC_Init(&NVIC_InitStructure);
	
	
	/* 使能TIM3工作 */
	TIM_Cmd(TIM3, ENABLE);	
}

void TIM3_IRQHandler(void)
{
	static uint32_t usart1_rx_cnt_last=0;
	
	//检测时间更新中断的标志位是否置位
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		if(g_usart1_rx_cnt)
		{
			//若相等，则表示串口3目前接收数据完毕
			if(g_usart1_rx_cnt == usart1_rx_cnt_last)
			{
				g_usart1_rx_end=1;
			}
			else
			{
				usart1_rx_cnt_last = g_usart1_rx_cnt;
			}
		}
		
		//清空标志位
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
