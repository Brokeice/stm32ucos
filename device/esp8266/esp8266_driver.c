#include "esp8266_driver.h"
#include "heartbeat.h"
#include "string.h"
#include "led.h"
#include "stdbool.h"
#include "lcd.h"
#include "lcd_helper.h"
#include "stm32f10x_usart.h"
#include "sys.h"
#include "misc.h"

ESP_UART_Info esp_uart_info;


bool espResponseOK=false;

//����2 PA2 TX PA3 RX
void esp8266_uart_init(u32 baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//��ʱ��
	RCC->APB2ENR|=1<<2;
	RCC->APB1ENR|=1<<17;
	
	//GPIO��ʼ�� �����������
	GPIOA->CRL&=~(0XFF<<8);
	GPIOA->CRL|=0X4B<<8;
	//�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
}


void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		res =USART_ReceiveData(USART2);	//��ȡ���յ�������
		
		esp_uart_info.buf[esp_uart_info.count++]=res;
		if((esp_uart_info.count>2)&&(esp_uart_info.buf[esp_uart_info.count-1]=='\n')&&(esp_uart_info.buf[esp_uart_info.count-2]=='\r'))
		{
			espProcess();
		}		
    } 

} 


void espSendStringToPC(char buf[])
{
	int i;
	for(i=0;i<strlen((char*)buf);i++)
	{
		if(buf[i]=='\0')
		{
			break;
		}
		USART2->DR =buf[i]; 
		while((USART2->SR&0X40)==0);
	}
	
}




//���ݽ��ճ�ʼ��
void espReceiveInit(void)
{
	memset(esp_uart_info.buf,0,ESP_MAX_RECEIVE_SIZE*sizeof(uint8_t));//��ʼ����������
	esp_uart_info.count=0;
	esp_uart_info.finish=false;
}


void espProcess()
{
	if(strstr((char*)esp_uart_info.buf,"OK")||strstr((char*)esp_uart_info.buf,"ready"))
	{
		espResponseOK=true;
		delay_ms(500);
	}
	espReceiveInit();
}






