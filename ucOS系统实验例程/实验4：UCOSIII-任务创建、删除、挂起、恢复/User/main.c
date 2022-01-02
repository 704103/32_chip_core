#include "system.h"
#include "led.h"
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
#define LED1_TASK_PRIO		4
//�����ջ��С	
#define LED1_STK_SIZE 		128
//������ƿ�
OS_TCB Led1TaskTCB;
//�����ջ	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);

//�������ȼ�
#define LED2_TASK_PRIO		5
//�����ջ��С	
#define LED2_STK_SIZE 		128
//������ƿ�
OS_TCB Led2TaskTCB;
//�����ջ	
CPU_STK LED2_TASK_STK[LED2_STK_SIZE];
void led2_task(void *p_arg);

//�������ȼ�
#define KEY_TASK_PRIO		6
//�����ջ��С	
#define KEY_STK_SIZE 		128
//������ƿ�
OS_TCB KEYTaskTCB;
//�����ջ	
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
void key_task(void *p_arg);

int main()
{  	
	OS_ERR err;
	
	LED_Init();
	KEY_Init();
	
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

	//����LED2����
	OSTaskCreate((OS_TCB 	* )&Led2TaskTCB,		
				 (CPU_CHAR	* )"led2 task", 		
                 (OS_TASK_PTR )led2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED2_TASK_PRIO,     
                 (CPU_STK   * )&LED2_TASK_STK[0],	
                 (CPU_STK_SIZE)LED2_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	
	//����KEY����
	OSTaskCreate((OS_TCB 	* )&KEYTaskTCB,		
				 (CPU_CHAR	* )"key task", 		
                 (OS_TASK_PTR )key_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEY_TASK_PRIO,     
                 (CPU_STK   * )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
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
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
	}
}

//led2������
void led2_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		led2=!led2;
		OSTimeDlyHMSM(0,0,0,400,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ400ms
	}
}

//KEY������
void key_task(void *p_arg)
{
	OS_ERR err;
	u8 key;
	p_arg = p_arg;
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY_UP: 
						OS_TaskSuspend((OS_TCB*)&Led2TaskTCB,&err);//����LED2����
						break;
			case KEY_DOWN: 
						OS_TaskResume((OS_TCB*)&Led2TaskTCB,&err);//�ָ�LED2����
						break;
			case KEY_LEFT: 
						OSTaskDel((OS_TCB*)&Led2TaskTCB,&err);//ɾ��LED2����
						break;
			case KEY_RIGHT: 
						OSTaskCreate((OS_TCB 	* )&Led2TaskTCB,  //����LED2����		
						 (CPU_CHAR	* )"led2 task", 		
						 (OS_TASK_PTR )led2_task, 			
						 (void		* )0,					
						 (OS_PRIO	  )LED2_TASK_PRIO,     
						 (CPU_STK   * )&LED2_TASK_STK[0],	
						 (CPU_STK_SIZE)LED2_STK_SIZE/10,	
						 (CPU_STK_SIZE)LED2_STK_SIZE,		
						 (OS_MSG_QTY  )0,					
						 (OS_TICK	  )0,					
						 (void   	* )0,					
						 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
						 (OS_ERR 	* )&err);
						break;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
	}
}

