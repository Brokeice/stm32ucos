#include "led.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"


//��ʼ��PC0-PC7
void led_init(void)
{
	RCC->APB2ENR|=1<<4;//��ʱ��
	GPIOC->CRL=0x3333333;//�������50MHz
	GPIOC->ODR|=0xFF;
}
