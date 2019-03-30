#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stdio.h"
#include "heartbeat.h"
#include "lcd.h"
#include "button.h"
#include "led.h"
#include "sys.h"
#include "uart.h"
#include "includes.h"
#include "misc.h"
#include "services.h"

#include "esp8266.h"

#include "lcd_helper.h"

/*
*docĿ¼���й��ڱ����̵�˵��
*/
void hardware_init(void);

int main(void)
{
	
	hardware_init();
	services_init();
}


void hardware_init(void)
{
	delay_init();
	button_init();
	led_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	uart_init(115200);
	LCD_Init();
	espInit(115200);
	
}






