#include "stm32f4xx.h"
#include "flash.h"
#include <stdio.h>

/**
 * @bieaf 扇区擦除
 *	
 * @param sector_num    扇区号
 * @return 1
 */
int32_t sector_erase(uint32_t sector_num)
{
	int32_t rt=-1;
	
	/* 解锁FLASH*/
	FLASH_Unlock();
	
	if(FLASH_COMPLETE==FLASH_EraseSector(sector_num,VoltageRange_3))
		rt=0;
	/* 锁定FLASH*/
	FLASH_Lock();
	
	return rt;
}


/**
 * @bieaf 写若干个数据
 *
 * @param addr       写入的地址
 * @param buf       写入数据的起始地址
 * @param word_size  长度
 * @return 
 */
void flash_program(uint32_t addr, uint32_t * buf, uint32_t word_size)
{	
	uint32_t i;
	
	/* 解锁FLASH*/
	FLASH_Unlock();
	
	for(i = 0; i < word_size; i++)	
	{
		/* 对FLASH烧写*/
		FLASH_ProgramWord( addr + 4 * i, buf[i]);	
	}

	/* 锁定FLASH*/
	FLASH_Lock();
}



/**
 * @bieaf 读若干个数据
 *
 * @param addr      读数据的地址
 * @param buf       读出数据的数组指针
 * @param word_size 长度
 * @return 
 */
void flash_read(uint32_t addr, uint32_t * buf, uint32_t word_size)
{
	uint32_t i=0;
	
	for(i =0; i < word_size; i++)
		buf[i] = *(__IO uint32_t*)(addr + 4 * i);
}


/* 读取启动模式 */
uint32_t read_start_mode(void)
{
	uint32_t mode = 0;
	
	flash_read((APPLICATION_2_ADDR + APPLICATION_2_SIZE - 4), &mode, 1);
	
	return mode;
}



/**
 * @bieaf 进行程序的覆盖
 * @detail 1.擦除目的地址
 *         2.源地址的代码拷贝到目的地址
 *         3.擦除源地址
 *
 * @param  搬运的源地址
 * @param  搬运的目的地址
 * @return 搬运的程序大小
 */
void move_code(uint32_t dest_addr, uint32_t src_addr,uint32_t word_size)
{
	uint32_t temp[256];
	uint32_t i;
	
	/*1.擦除目的地址*/
	printf("> start erase application 1 sector......\r\n");
	
	//擦除
	sector_erase(APPLICATION_1_SECTOR);
	
	printf("> erase application 1 success......\r\n");
	
	/*2.开始拷贝*/	

	printf("> start copy......\r\n");
	
	for(i = 0; i <word_size/1024; i++)
	{
		flash_read((src_addr + i*1024), temp, 256);
		flash_program((dest_addr + i*1024), temp, 256);
	}
	
	printf("> copy finish......\r\n");
	
	/*3.擦除源地址*/
	
	printf("> start erase application 2 sector......\r\n");
	
	//擦除
	sector_erase(APPLICATION_2_SECTOR);
	
	printf("> erase application 2 success......\r\n");
	
}

/* 采用汇编设置栈的值 */
__asm void MSR_MSP (uint32_t ulAddr) 
{
    MSR MSP, r0 			                   //set Main Stack value
    BX r14
}





void iap_execute_app (uint32_t app_addr)
{
	jump_func jump_to_app; 
	
	//printf("* ( __IO uint32_t * ) app_addr  =%08X ,app_addr=%08X\r\n",* ( __IO uint32_t * ) app_addr,app_addr );
    
	//if ( ( ( * ( __IO uint32_t * ) app_addr ) & 0x2FFE0000 ) == 0x200006B0 )	//检查栈顶地址是否合法.
	//{ 
		//printf("stack is legal\r\n");
		
		jump_to_app = (jump_func) * ( __IO uint32_t *)(app_addr + 4);			//用户代码区第二个字为程序开始地址(复位地址)		
		
		MSR_MSP( * ( __IO uint32_t * ) app_addr );								//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		
		jump_to_app();															//跳转到APP.
	//}
	
	//printf("stack is illegal\r\n");
}
