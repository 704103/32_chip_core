#include "system.h"
#include "led.h"
#include "usart.h"
#include "includes.h"
#include "key.h"


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
#define MYTMR_TASK_PRIO		4
//�����ջ��С	
#define MYTMR_STK_SIZE 		128
//������ƿ�
OS_TCB MYTMRTaskTCB;
//�����ջ	
CPU_STK MYTMR_TASK_STK[MYTMR_STK_SIZE];
void MyTmr_task(void *p_arg);

//�������ȼ�
#define LED1_TASK_PRIO		5
//�����ջ��С	
#define LED1_STK_SIZE 		128
//������ƿ�
OS_TCB Led1TaskTCB;
//�����ջ	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);


CPU_TS	ts_start;       //ʱ�������
CPU_TS  ts_end; 
u8 time=0;

int main()
{  	
	OS_ERR err;
	
	LED_Init();
	KEY_Init();
	USART1_Init(9600);
	
	OSInit(&err);		//��ʼ��UCOSIII
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
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	
	CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
	
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
	
	cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();       
	
	
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
	//����LED1����
	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,		
				 (CPU_CHAR	* )"led1 task", 		
                 (OS_TASK_PTR )led1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     
                 (CPU_STK   * )&LED1_TASK_STK[0],	
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

	//���������ʱ������
	OSTaskCreate((OS_TCB 	* )&MYTMRTaskTCB,		
				 (CPU_CHAR	* )"MyTmr_task", 		
                 (OS_TASK_PTR )MyTmr_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MYTMR_TASK_PRIO,     
                 (CPU_STK   * )&MYTMR_TASK_STK[0],	
                 (CPU_STK_SIZE)MYTMR_STK_SIZE/10,	
                 (CPU_STK_SIZE)MYTMR_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led1������
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		led1=!led1;
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err); 
	}
}

void MyTmrCallback(OS_TMR *p_tmr,void *p_arg)  //�����������ʱ���ص�����
{
	CPU_INT32U  cpu_clk_freq;
	
	CPU_SR_ALLOC();

	printf("\r\n���ݵĲ�����Ϣ�ǣ�%s\r\n",(char *)p_arg);

	cpu_clk_freq = BSP_CPU_ClkFreq(); 
	
	led2=!led2;
	ts_end=OS_TS_GET()-ts_start;
	
	OS_CRITICAL_ENTER(); 
	
	printf("\r\n��ʱ1�룬ͨ��ʱ�����ö�ʱ�ǣ�%8d us���� %4d ms\r\n",
			ts_end/(cpu_clk_freq/1000000),ts_end/(cpu_clk_freq/1000));
	time++;
	OS_CRITICAL_EXIT();
	ts_start=OS_TS_GET();
	
}

//�����ʱ��������
void MyTmr_task(void *p_arg)
{
	OS_ERR err;
	OS_TMR  my_tmr;   //���������ʱ������
	
	p_arg = p_arg;
	
	//���������ʱ��
	OSTmrCreate((OS_TMR              *)&my_tmr,        //�����ʱ������
               (CPU_CHAR            *)"MyTmr",       //���������ʱ��
               (OS_TICK              )10,            //��ʱ����ʼֵ����10Hzʱ�����㣬��Ϊ1s
               (OS_TICK              )10,            //��ʱ����������ֵ����10Hzʱ�����㣬��Ϊ1s
               (OS_OPT               )OS_OPT_TMR_PERIODIC, //�����Զ�ʱ
               (OS_TMR_CALLBACK_PTR  )MyTmrCallback,         //�ص�����
               (void                *)"Timer Over!",       //����ʵ�θ��ص�����
               (OS_ERR              *)err);                //���ش�������
			   
	OSTmrStart((OS_TMR              *)&my_tmr,
			   (OS_ERR              *)err);	
	
	
	ts_start=OS_TS_GET();       //��ȡ��ʱǰʱ���
				
	while(1)
	{
		
		if(time==5)  //��ʱ5���ɾ�������ʱ��
		{
			time=0;
			OSTmrDel(&my_tmr,&err);
		}
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err); 
	}
}
