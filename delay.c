#include "stm32f4xx.h"
#include "sys.h"



void delay_us(uint32_t n)
{

	SysTick->CTRL = 0; // 关闭系统定时器
	SysTick->LOAD = (SystemCoreClock/1000000*n)-1; // 设置计数值168-1,就是从167 ~ 0 就是168次的计数，1us时间的到达
	SysTick->VAL = 0;  // 清零计数值也会去清空标志位
	SysTick->CTRL = 5; // 使能系统定时器工作，且时钟源为处理器时钟（HCLK），168MHz
	while ((SysTick->CTRL & 0x10000)==0);// 若计数值减到0，SysTick->CTRL寄存器的COUNTFLAG会置1，就是计数完毕

	SysTick->CTRL = 0; // 关闭系统定时器
}


void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; // 关闭系统定时器
		SysTick->LOAD = (SystemCoreClock/1000)-1; // 设置计数值168000-1,就是从167999 ~ 0 就是168000次的计数，1ms时间的到达
		SysTick->VAL = 0;  // 清零计数值也会去清空标志位
		SysTick->CTRL = 5; // 使能系统定时器工作，且时钟源为处理器时钟（HCLK），168MHz
		while ((SysTick->CTRL & 0x10000)==0);// 若计数值减到0，SysTick->CTRL寄存器的COUNTFLAG会置1，就是计数完毕
	}
	
	SysTick->CTRL = 0; // 关闭系统定时器
}
