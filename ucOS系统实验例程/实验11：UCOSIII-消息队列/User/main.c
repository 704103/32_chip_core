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
#define QUEUEWR_TASK_PRIO		6
//�����ջ��С	
#define QUEUEWR_STK_SIZE 		128
//������ƿ�
OS_TCB QUEUEWRTaskTCB;
//�����ջ	
CPU_STK QUEUEWR_TASK_STK[QUEUEWR_STK_SIZE];
void QueueWr_task(void *p_arg);


//�������ȼ�
#define QUEUERD_TASK_PRIO		7
//�����ջ��С	
#define QUEUERD_STK_SIZE 		128
//������ƿ�
OS_TCB QUEUERDTaskTCB;
//�����ջ	
CPU_STK QUEUERD_TASK_STK[QUEUERD_STK_SIZE];
void QueueRd_task(void *p_arg);


OS_Q My_queue;

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
	
	//������Ϣ���� My_queue
    OSQCreate((OS_Q *)&My_queue,    
               (CPU_CHAR    *)"My_queue",    
			   (OS_MSG_QTY ) 10,    //�������Ϣ����Ŀ
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
	OSTaskCreate((OS_TCB 	* )&QUEUEWRTaskTCB,		
				 (CPU_CHAR	* )"QueueWr_task", 		
                 (OS_TASK_PTR )QueueWr_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )QUEUEWR_TASK_PRIO,     
                 (CPU_STK   * )&QUEUEWR_TASK_STK[0],	
                 (CPU_STK_SIZE)QUEUEWR_STK_SIZE/10,	
                 (CPU_STK_SIZE)QUEUEWR_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
		
	//��������
	OSTaskCreate((OS_TCB 	* )&QUEUERDTaskTCB,		
				 (CPU_CHAR	* )"QueueRd_task", 		
                 (OS_TASK_PTR )QueueRd_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )QUEUERD_TASK_PRIO,     
                 (CPU_STK   * )&QUEUERD_TASK_STK[0],	
                 (CPU_STK_SIZE)QUEUERD_STK_SIZE/10,	
                 (CPU_STK_SIZE)QUEUERD_STK_SIZE,		
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
void QueueWr_task(void *p_arg)
{
	OS_ERR err;
	
	p_arg = p_arg;
				
	while(1)
	{
		OSQPost(&My_queue,
				"www.prechin.cn",
				sizeof("www.prechin.cn"),
				OS_OPT_POST_FIFO | OS_OPT_POST_ALL,
				&err);
		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);	
		
	}
}


//��������
void QueueRd_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	char *pmsg;
	CPU_SR_ALLOC();
	p_arg = p_arg;
				
	while(1)
	{
		pmsg=OSQPend(&My_queue,0,OS_OPT_PEND_BLOCKING,&msg_size,0,&err);
		if(err==OS_ERR_NONE)  //��Ϣ���ճɹ�
		{
			OS_CRITICAL_ENTER();	//�����ٽ���
			printf("���յ���Ϣ�����ǣ�%d�ֽڣ���Ϣ�����ǣ�%s\r\n",msg_size,pmsg);
			OS_CRITICAL_EXIT();     //�˳��ٽ���
		}
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err); 
	}
}




