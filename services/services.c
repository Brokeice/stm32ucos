#include "stdio.h"
#include "includes.h"
#include "log.h"
#include "devices.h"
#include "services.h"

u8 create_task(void (*function)(void *params),int priorty,CPU_STK stk[],OS_TCB tcb);






//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define LED0_TASK_PRIO		7
//�����ջ��С	
#define LED0_STK_SIZE 		128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

#define BUTTON_SCAN_TASK_PRIO 6
#define BUTTON_SCAN_STK_SIZE  128
OS_TCB  buttonScanTaskTCB;
CPU_STK BUTTON_SCAN_TASK_STK[BUTTON_SCAN_STK_SIZE];
void button_scan_task(void *params);


#define UART1_TASK_PRIO      4
#define UART1_STK_SIZE       128
CPU_STK UART1_TASK_STK[UART1_STK_SIZE];
OS_TCB  uart1TaskTCB;
void uart1Task(void *params);


#define LOG_TASK_PRIO      5
#define LOG_STK_SIZE       128
CPU_STK LOG_TASK_STK[LOG_STK_SIZE];
OS_TCB  logTaskTCB;
void logTask(void *params);



#define CLOCK_TASK_PRIO      8
#define CLOCK_STK_SIZE       128
CPU_STK CLOCK_TASK_STK[CLOCK_STK_SIZE];
OS_TCB  clockTaskTCB;
void clockTask(void *params);





void services_init(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
}


//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����LED0����
	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
				 (CPU_CHAR	* )"led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK   * )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//������������
	OSTaskCreate((OS_TCB 	* )&buttonScanTaskTCB,		
				 (CPU_CHAR	* )"button task", 		
                 (OS_TASK_PTR )button_scan_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )BUTTON_SCAN_TASK_PRIO,     
                 (CPU_STK   * )&BUTTON_SCAN_TASK_STK[0],	
                 (CPU_STK_SIZE)BUTTON_SCAN_STK_SIZE/10,	
                 (CPU_STK_SIZE)BUTTON_SCAN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
    //������������
    OSTaskCreate(
				 (OS_TCB     *)&uart1TaskTCB,
				 (CPU_CHAR   *)"uart1 task",
				 (OS_TASK_PTR )uart1Task,
				 (void       *)0,
				 (OS_PRIO     )UART1_TASK_PRIO,
				 (CPU_STK    *)&UART1_TASK_STK[0],
				 (CPU_STK_SIZE)UART1_STK_SIZE/10,
				 (CPU_STK_SIZE)UART1_STK_SIZE,
				 (OS_MSG_QTY  )0,
				 (OS_TICK     )0,
				 (void       *)0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR     *)&err);
	//������־����
    OSTaskCreate(
				 (OS_TCB     *)&logTaskTCB,
				 (CPU_CHAR   *)"log task",
				 (OS_TASK_PTR )logTask,
				 (void       *)0,
				 (OS_PRIO     )LOG_TASK_PRIO,
				 (CPU_STK    *)&LOG_TASK_STK[0],
				 (CPU_STK_SIZE)LOG_STK_SIZE/10,
				 (CPU_STK_SIZE)LOG_STK_SIZE,
				 (OS_MSG_QTY  )0,
				 (OS_TICK     )0,
				 (void       *)0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR     *)&err);
	//����ʱ������
    OSTaskCreate(
				 (OS_TCB     *)&clockTaskTCB,
				 (CPU_CHAR   *)"clock task",
				 (OS_TASK_PTR )clockTask,
				 (void       *)0,
				 (OS_PRIO     )CLOCK_TASK_PRIO,
				 (CPU_STK    *)&CLOCK_TASK_STK[0],
				 (CPU_STK_SIZE)CLOCK_STK_SIZE/10,
				 (CPU_STK_SIZE)CLOCK_STK_SIZE,
				 (OS_MSG_QTY  )0,
				 (OS_TICK     )0,
				 (void       *)0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR     *)&err);
//	create_task(clockTask,CLOCK_TASK_PRIO,CLOCK_TASK_STK,clockTaskTCB);		 
	
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//Ϊ�˷��㴴������д���������������������
//function 		:ִ������ĺ��� 
//prority  		:�������ȼ�
//stk      		:�����ջ
//tcb      		:�����
//return @err	:����0��ʾ���񴴽��ɹ� 
u8 create_task(void (*function)(void *params),int priorty,CPU_STK stk[],OS_TCB tcb)
{
	static int i=0;
	char task_char[20]={0};
	int default_stk_size=128;
	CPU_ERR err;
	sprintf(task_char,"task %d",i++);
	//����ʱ������
    OSTaskCreate(
				 (OS_TCB     *)&tcb,
				 (CPU_CHAR   *)task_char,
				 (OS_TASK_PTR )&function,
				 (void       *)0,
				 (OS_PRIO     )priorty,
				 (CPU_STK    *)&stk[0],
				 (CPU_STK_SIZE)default_stk_size/10,
				 (CPU_STK_SIZE)default_stk_size,
				 (OS_MSG_QTY  )0,
				 (OS_TICK     )0,
				 (void       *)0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR     *)&err);
	return err;
}


//led0������
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		showLog("LED3 ON ");
		LED3=0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		showLog("LED3 OFF");
		LED3=1;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

void uart1Task(void *params)
{
	OS_ERR err;
	int i=0;
	while(1)
	{
		if(uart_info.finish==true)
		{
			printf("%s",uart_info.buf);
			POINT_COLOR=RED;
			LCD_ShowMyString(0,i,16,(u8*)uart_info.buf);
			receiveInit();
			i+=16;
		}
		OSTimeDlyHMSM(0,0,0,80,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
	}	
}
//������־����
void logTask(void *params)
{
	OS_ERR err;
	while(1)
	{
		if(logFlag==true)
		{
			printf("Debug Log:%s",log_info.info);
			POINT_COLOR=RED;
			LCD_ShowMyString(0,MIDDLE_LINE_12,16,(u8*)log_info.info);
			logFlag=false;
		}
		OSTimeDlyHMSM(0,0,0,80,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


//����ɨ��
void button_scan_task(void *params)
{
	OS_ERR err;
	while(1)
	{
		u8 key=button_scan(1);
		switch(key)
		{
			case right_press:
				espContinueTransportInit();
				LCD_ShowMyString(0,MIDDLE_LINE_13,16,(u8*)"Enter continue mode");
			    
			break;
			
			case down_press:
				espContinueTransportExit();
				LCD_ShowMyString(0,MIDDLE_LINE_13,16,(u8*)"Exit continue mode ");
			break;
			
			case left_press:
				espSendString("Left   press\r\n");
				LCD_ShowMyString(0,MIDDLE_LINE_13,16,(u8*)"Left   press       ");
			break;
			
			case up_press:
				espSendString("Up   press\r\n");
				LCD_ShowMyString(0,MIDDLE_LINE_13,16,(u8*)"Up     press       ");
			break;
			
			default:
				
			break;
		}
		OSTimeDlyHMSM(0,0,0,80,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ80ms
	}
		
		
}

//ϵͳʱ�ӷ���
void clockTask(void *params)
{
	OS_ERR err;
	while(1)
	{
		char *timestamp=getSystemTime();
		LCD_ShowMyString(0,MIDDLE_LINE_0,16,(u8*)timestamp);
		if(esp_state.continueMode==true)
		{
			espSendString(timestamp);
		}
		OSTimeDlyHMSM(0,0,0,400,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ800ms
	}
}











