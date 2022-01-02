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
#define TASKQPOST_TASK_PRIO		6
//�����ջ��С	
#define TASKQPOST_STK_SIZE 		128
//������ƿ�
OS_TCB TASKQPOSTTaskTCB;
//�����ջ	
CPU_STK TASKQPOST_TASK_STK[TASKQPOST_STK_SIZE];
void TaskQPost_task(void *p_arg);


//�������ȼ�
#define TASKQPEND_TASK_PRIO		7
//�����ջ��С	
#define TASKQPEND_STK_SIZE 		128
//������ƿ�
OS_TCB TASKQPENDTaskTCB;
//�����ջ	
CPU_STK TASKQPEND_TASK_STK[TASKQPEND_STK_SIZE];
void TaskQPend_task(void *p_arg);

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
				 
	//��������
	OSTaskCreate((OS_TCB 	* )&TASKQPOSTTaskTCB,		
				 (CPU_CHAR	* )"TaskQPost_task", 		
                 (OS_TASK_PTR )TaskQPost_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASKQPOST_TASK_PRIO,     
                 (CPU_STK   * )&TASKQPOST_TASK_STK[0],	
                 (CPU_STK_SIZE)TASKQPOST_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASKQPOST_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
		
	//��������
	OSTaskCreate((OS_TCB 	* )&TASKQPENDTaskTCB,		
				 (CPU_CHAR	* )"TaskQPend_task", 		
                 (OS_TASK_PTR )TaskQPend_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASKQPEND_TASK_PRIO,     
                 (CPU_STK   * )&TASKQPEND_TASK_STK[0],	
                 (CPU_STK_SIZE)TASKQPEND_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASKQPEND_STK_SIZE,		
                 (OS_MSG_QTY  )50,			//����ɽ��յ������Ϣ��		
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
void TaskQPost_task(void *p_arg)
{
	OS_ERR err;

	p_arg = p_arg;

			
	while(1)
	{
		
		//������Ϣ������ TASKQPENDTaskTCB
		OSTaskQPost ((OS_TCB      *)&TASKQPENDTaskTCB,           //Ŀ������Ŀ��ƿ�
					(void        *)"www.prechin.cn",             //��Ϣ����
					(OS_MSG_SIZE  )sizeof ( "www.prechin.cn" ),  //��Ϣ����
					(OS_OPT       )OS_OPT_POST_FIFO,        //������������Ϣ���е���ڶ�
					(OS_ERR      *)&err);                           //���ش�������
		
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
		
	}
}


//��������
void TaskQPend_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	CPU_TS         ts;
	CPU_INT32U     cpu_clk_freq;
	char * pMsg;
	
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	cpu_clk_freq = BSP_CPU_ClkFreq();        //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���
				
	while(1)
	{
		//�������񣬵ȴ�������Ϣ
		pMsg = OSTaskQPend ((OS_TICK        )0,                    //�����޵ȴ�
							(OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
							(OS_MSG_SIZE   *)&msg_size,            //������Ϣ����
							(CPU_TS        *)&ts,                  //������Ϣ��������ʱ���
							(OS_ERR        *)&err);                //���ش�������

		ts = OS_TS_GET() - ts;                //�����ź����ӷ��������յ�ʱ���
		led2=!led2;
		
		OS_CRITICAL_ENTER();	//�����ٽ���
		printf("���յ�����Ϣ������Ϊ��%s�������ǣ�%d�ֽ�\r\n",pMsg,msg_size);
		printf("������Ϣ�ӱ������������յ�ʱ����ǣ�%d us\r\n",ts/(cpu_clk_freq / 1000000));
		OS_CRITICAL_EXIT();	//�˳��ٽ���
	
	}
}
