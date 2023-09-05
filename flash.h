#ifndef __FLASH_H__
#define __FLASH_H__



/*=====用户配置(根据自己的分区进行配置)=====*/
#define BootLoader_Size 		0x4000U			///< BootLoader的大小 16K
#define Application_Size		0x20000U		///< 应用程序的大小 128K

#define APPLICATION_1_ADDR		0x08020000U		///< 应用程序1的首地址，扇区5首地址
#define APPLICATION_2_ADDR		0x08040000U		///< 应用程序2的首地址，扇区6首地址

#define APPLICATION_1_SECTOR	FLASH_Sector_5	///< 应用程序1的扇区
#define APPLICATION_2_SECTOR	FLASH_Sector_6	///< 应用程序2的扇区


#define APPLICATION_1_SIZE		0x20000U		///< 应用程序1的大小
#define APPLICATION_2_SIZE		0x20000U		///< 应用程序2的大小

/*==========================================*/


/* 启动的步骤 */
#define STARTUP_NORMAL 0xFFFFFFFF	///< 正常启动
#define STARTUP_UPDATE 0xAAAAAAAA	///< 升级再启动
#define STARTUP_RESET  0x5555AAAA	///< 恢复出厂,目前没使用


/* 程序跳转函数 */
typedef void (*jump_func)(void);


extern void flash_program(uint32_t addr, uint32_t * buf, uint32_t word_size);

extern void flash_read(uint32_t addr, uint32_t * buf, uint32_t word_size);

extern int32_t sector_erase(uint32_t sector_num);

extern uint32_t read_start_mode(void);

extern void move_code(uint32_t dest_addr, uint32_t src_addr,uint32_t word_size);

extern void iap_execute_app (uint32_t app_addr);


#endif
