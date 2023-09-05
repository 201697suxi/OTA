#ifndef __USART_H__
#define __USART_H__


extern volatile uint8_t  g_usart1_rx_buf[1280];
extern volatile uint32_t g_usart1_rx_cnt;
extern volatile uint32_t g_usart1_rx_end;



extern void usart1_init(uint32_t baud);
extern void usart3_init(uint32_t baud);

extern void usart3_send_str(char *str);
extern void usart3_send_bytes(uint8_t *buf,uint32_t len);

extern void usart1_send_bytes(uint8_t *buf,uint32_t len);


#endif



