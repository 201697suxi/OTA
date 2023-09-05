#ifndef __LED_H__
#define __LED_H__

#define CONNECT_MQTT_LED(x)	PEout(14)=(x)?0:1

extern void led_init(void);




#endif
