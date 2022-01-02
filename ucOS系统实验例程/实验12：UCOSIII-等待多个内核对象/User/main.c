#include "system.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "includes.h"



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
#define LED1_TASK_PRIO		4
//�����ջ��С	
#define LED1_STK_SIZE 		128
//������ƿ�
OS_TCB Led1TaskTCB;
//�����ջ	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);


//�������ȼ�
#define MULTIPOST_TASK_PRIO		6
//�����ջ��С	
#define MULTIPOST_STK_SIZE 		128
//������ƿ�
OS_TCB MULTIPOSTTaskTCB;
//�����ջ	
CPU_STK MULTIPOST_TASK_STK[MULTIPOST_STK_SIZE];
void MultiPost_task(void *p_arg);


//�������ȼ�
#define MULTIPEND_TASK_PRIO		7
//�����ջ��С	
#define MULTIPEND_STK_SIZE 		128
//������ƿ�
OS_TCB MULTIPENDTaskTCB;
//�����ջ	
CPU_STK MULTIPEND_TASK_STK[MULTIPEND_STK_SIZE];
void MultiPend_task(void *p_arg);


OS_SEM my_sem;
OS_Q my_q;
OS_PEND_DATA multi_pend_data[2];

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
	
	OSSemCreate(&my_sem,"my_sem",0,&err);  
	
	OSQCreate(&my_q,"my_q",10,&err);
	
	//��ʼ��Ҫ�ȴ��Ķ���ں˶���
	multi_pend_data[0].PendObjPtr=(OS_PEND_OBJ *)&my_sem;
	multi_pend_data[1].PendObjPtr=(OS_PEND_OBJ *)&my_q;
	
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
				 
	
	//��������
	OSTaskCreate((OS_TCB 	* )&MULTIPOSTTaskTCB,		
				 (CPU_CHAR	* )"MultiPost_task", 		
                 (OS_TASK_PTR )MultiPost_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MULTIPOST_TASK_PRIO,     
                 (CPU_STK   * )&MULTIPOST_TASK_STK[0],	
                 (CPU_STK_SIZE)MULTIPOST_STK_SIZE/10,	
                 (CPU_STK_SIZE)MULTIPOST_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
		
	//��������
	OSTaskCreate((OS_TCB 	* )&MULTIPENDTaskTCB,		
				 (CPU_CHAR	* )"MultiPend_task", 		
                 (OS_TASK_PTR )MultiPend_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MULTIPEND_TASK_PRIO,     
                 (CPU_STK   * )&MULTIPEND_TASK_STK[0],	
                 (CPU_STK_SIZE)MULTIPEND_STK_SIZE/10,	
                 (CPU_STK_SIZE)MULTIPEND_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

//led1������
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		led1=!led1;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err); //��ʱ200ms
	}
}

void MyTmrCallback(OS_TMR *p_tmr,void *p_arg)
{
	OS_ERR err;
	
	if(KEY_Scan(0)==KEY_UP)
	{
		OS_SemPost(&my_sem,OS_OPT_POST_ALL,0,&err);
		
	}
}

//��������
void MultiPost_task(void *p_arg)
{
	OS_ERR err;

	OS_TMR my_tmr;

	p_arg = p_arg;
	
	OSTmrCreate(&my_tmr,"my_tmr",0,1,OS_OPT_TMR_PERIODIC,(void *)MyTmrCallback,"Time Over!",&err);
	OSTmrStart(&my_tmr,&err);
			
	while(1)
	{
		
		OSQPost(&my_q,
				"www.prechin.cn",
				sizeof("www.prechin.cn"),
				OS_OPT_POST_FIFO | OS_OPT_POST_ALL,
				&err
				);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
		
	}
}


//��������
void MultiPend_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
				
	while(1)
	{
		OSPendMulti(multi_pend_data,2,0,OS_OPT_PEND_BLOCKING,&err);
		
		if(multi_pend_data[0].RdyObjPtr==multi_pend_data[0].PendObjPtr)
		{
			OS_CRITICAL_ENTER();	//�����ٽ���
			printf("����K_UP�����£���ֵ�ź���������\r\n");
			OS_CRITICAL_EXIT();	//�˳��ٽ���
		}
		
		if(multi_pend_data[1].RdyObjPtr==multi_pend_data[1].PendObjPtr)
		{
			OS_CRITICAL_ENTER();	//�����ٽ���
			printf("���յ�һ����Ϣ����Ϣ����Ϊ��%s,��Ϣ����Ϊ��%d�ֽ�\r\n",multi_pend_data[1].RdyMsgPtr,multi_pend_data[1].RdyMsgSize);
			OS_CRITICAL_EXIT();	//�˳��ٽ���
		}
	
	}
}




