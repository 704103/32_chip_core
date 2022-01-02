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
#define MUTEXWR_TASK_PRIO		6
//�����ջ��С	
#define MUTEXWR_STK_SIZE 		128
//������ƿ�
OS_TCB MUTEXWRTaskTCB;
//�����ջ	
CPU_STK MUTEXWR_TASK_STK[MUTEXWR_STK_SIZE];
void MutexWr_task(void *p_arg);


//�������ȼ�
#define MUTEXRD_TASK_PRIO		7
//�����ջ��С	
#define MUTEXRD_STK_SIZE 		128
//������ƿ�
OS_TCB MUTEXRDTaskTCB;
//�����ջ	
CPU_STK MUTEXRD_TASK_STK[MUTEXRD_STK_SIZE];
void MutexRd_task(void *p_arg);

OS_MUTEX My_mutex;

static u8 mutex_test[2];

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
	
	//���������ź��� My_mutex
    OSMutexCreate((OS_MUTEX *)&My_mutex,    //ָ���ź���������ָ��
               (CPU_CHAR    *)"My_mutex",    //�ź���������
               (OS_ERR      *)&err);         //��������
	
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
	OSTaskCreate((OS_TCB 	* )&MUTEXWRTaskTCB,		
				 (CPU_CHAR	* )"MutexWr_task", 		
                 (OS_TASK_PTR )MutexWr_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MUTEXWR_TASK_PRIO,     
                 (CPU_STK   * )&MUTEXWR_TASK_STK[0],	
                 (CPU_STK_SIZE)MUTEXWR_STK_SIZE/10,	
                 (CPU_STK_SIZE)MUTEXWR_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
		
	//��������
	OSTaskCreate((OS_TCB 	* )&MUTEXRDTaskTCB,		
				 (CPU_CHAR	* )"MutexRd_task", 		
                 (OS_TASK_PTR )MutexRd_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MUTEXRD_TASK_PRIO,     
                 (CPU_STK   * )&MUTEXRD_TASK_STK[0],	
                 (CPU_STK_SIZE)MUTEXRD_STK_SIZE/10,	
                 (CPU_STK_SIZE)MUTEXRD_STK_SIZE,		
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

//��������
void MutexWr_task(void *p_arg)
{
	OS_ERR err;
	
	p_arg = p_arg;
				
	while(1)
	{
		OSMutexPend(&My_mutex,0,OS_OPT_PEND_BLOCKING,0,&err);
		
		mutex_test[0]++;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
		mutex_test[1]++;
		
		OSMutexPost(&My_mutex,OS_OPT_POST_NONE,&err);
		
	}
}


//��������
void MutexRd_task(void *p_arg)
{
	OS_ERR err;

	CPU_SR_ALLOC();
	p_arg = p_arg;
				
	while(1)
	{
		OSMutexPend(&My_mutex,0,OS_OPT_PEND_BLOCKING,0,&err);
		
		if(mutex_test[0]==mutex_test[1])
		{
			OS_CRITICAL_ENTER();	//�����ٽ���
			printf("�������ݳɹ���\r\n");
			OS_CRITICAL_EXIT();     //�˳��ٽ���
		}
		else
		{
			OS_CRITICAL_ENTER();	//�����ٽ���
			printf("�������ݴ���\r\n");
			OS_CRITICAL_EXIT();     //�˳��ٽ���
		}
		OSMutexPost(&My_mutex,OS_OPT_POST_NONE,&err);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err); 
	}
}




