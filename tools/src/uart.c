#include "sys.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stdio.h"
#include "stdbool.h"
#include "devices.h"


int fputc(int ch, FILE *f)
{      
	USART1->DR = (u8) ch; 
	while((USART1->SR&0X40)==0){};//ѭ������,ֱ���������   
	return ch;
}

UART_Info uart_info;
u8 responseFlag=0;


//����1 PA9 TX PA10 RX
void uart_init(u32 baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//��ʱ��
	RCC->APB2ENR|=1<<14;
	RCC->APB2ENR|=1<<2;
	//GPIO��ʼ�� �����������
	GPIOA->CRH&=~(0XFF<<4);
	GPIOA->CRH|=0X4B<<4;
	//�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 res,mode=1;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		USART_SendData(USART1,res);
		
		if(mode==0)
		{
			if(uart_info.count<100)
			{
				uart_info.buf[uart_info.count++]=res;
				
			}
			else if(uart_info.finish==false)
			{
				uart_info.finish=true;
			}
		}
		if(mode==1)
		{
			uart_info.buf[uart_info.count++]=res;
			if((uart_info.count>2)&&(uart_info.buf[uart_info.count-1]=='\n')&&(uart_info.buf[uart_info.count-2]=='\r'))
			{
				uart_info.finish=true;
				//process();
			}
		}
			
    } 

} 



//���ݽ��ճ�ʼ��
void receiveInit(void)
{
	memset(uart_info.buf,0,MAX_RECEIVE_SIZE*sizeof(uint8_t));//��ʼ����������
	uart_info.count=0;
	uart_info.finish=false;
}
//����λ��������
void sendToPC(uint8_t buf[])
{
	int i;
	int len=strlen((char*)buf)+1;
	uint8_t sep[]="\r\n";
	for(i=0;i<len;i++)
	{
		if(i%20==0&&(i!=0))
		{
		delay_ms(5000);
		sendStringToPC(sep);
		}
		USART_SendData(USART1,buf[i]);
		while((USART1->SR&0X40)==0);	
	}
	receiveInit();
	
}

void sendStringToPC(uint8_t buf[])
{
	int i;
	for(i=0;i<strlen((char*)buf);i++)
	{
		if(buf[i]=='\0')
		{
			break;
		}
		USART1->DR =buf[i]; 
		while((USART1->SR&0X40)==0);
	}
	
}


