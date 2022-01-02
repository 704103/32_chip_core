#include "system.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
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
#define USART_TASK_PRIO		5
//�����ջ��С	
#define USART_STK_SIZE 		512
//������ƿ�
OS_TCB USARTTaskTCB;
//�����ջ	
CPU_STK USART_TASK_STK[USART_STK_SIZE];
void Usart_task(void *p_arg);


//�������ȼ�
#define KEY_TASK_PRIO		5
//�����ջ��С	
#define KEY_STK_SIZE 		512
//������ƿ�
OS_TCB KEYTaskTCB;
//�����ջ	
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
void Key_task(void *p_arg);



OS_MEM mymem;
u8 ucArray [ 70 ] [ 4 ];   //�����ڴ������С


int main()
{  	
	OS_ERR err;
	
	LED_Init();
	KEY_Init();
	My_EXTI_Init();
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
	
	//�����ڴ������� mymem 
	OSMemCreate ((OS_MEM      *)&mymem,             //ָ���ڴ�������
				(CPU_CHAR    *)"mymem",   //�����ڴ�������
				(void        *)ucArray,          //�ڴ�������׵�ַ
				(OS_MEM_QTY   )70,               //�ڴ�������ڴ����Ŀ
				(OS_MEM_SIZE  )4,                //�ڴ����ֽ���Ŀ
				(OS_ERR      *)&err);            //���ش�������		
				
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
	OSTaskCreate((OS_TCB 	* )&USARTTaskTCB,		
				 (CPU_CHAR	* )"Usart task", 		
                 (OS_TASK_PTR )Usart_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )USART_TASK_PRIO,     
                 (CPU_STK   * )&USART_TASK_STK[0],	
                 (CPU_STK_SIZE)USART_STK_SIZE/10,	
                 (CPU_STK_SIZE)USART_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
		
	//��������
	OSTaskCreate((OS_TCB 	* )&KEYTaskTCB,		
				 (CPU_CHAR	* )"Key task", 		
                 (OS_TASK_PTR )Key_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEY_TASK_PRIO,     
                 (CPU_STK   * )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
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
void Usart_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	char * pMsg;
	
	CPU_SR_ALLOC();
	
	p_arg = p_arg;
		
	while(1)
	{
		//�������񣬵ȴ�������Ϣ */
		pMsg = OSTaskQPend ((OS_TICK        )0,                    //�����޵ȴ�
							(OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
							(OS_MSG_SIZE   *)&msg_size,            //������Ϣ����
							(CPU_TS        *)0,                    //������Ϣ��������ʱ���
							(OS_ERR        *)&err);                //���ش�������

		OS_CRITICAL_ENTER();                //�����ٽ�Σ����⴮�ڴ�ӡ�����

		printf ( "%c", * pMsg );            //��ӡ��Ϣ����

		OS_CRITICAL_EXIT();              //�˳��ٽ��
		
		//�˻��ڴ��
		OSMemPut ((OS_MEM  *)&mymem,              //ָ���ڴ�������
							(void    *)pMsg,    //�ڴ����׵�ַ
							(OS_ERR  *)&err);	//���ش�������
		
	
		
	}
}


//��������
void Key_task(void *p_arg)
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT16U     version;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	
	p_arg = p_arg;
	
	version = OSVersion(&err);            //��ȡuC/OS�汾��
	
	cpu_clk_freq = BSP_CPU_ClkFreq();     //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���
	
	while(1)
	{
		//��������ֱ��K_UP������
		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
						(OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
						(CPU_TS   *)0,                     //��ȡ�ź�����������ʱ���
						(OS_ERR   *)&err);                 //���ش�������
		
		ts_int = CPU_IntDisMeasMaxGet ();                 //��ȡ�����ж�ʱ��

		OS_CRITICAL_ENTER();                              //�����ٽ�Σ����⴮�ڴ�ӡ�����

		printf ( "\r\nuC/OS�汾�ţ�V%d.%02d.%02d\r\n",
				version / 10000, version % 10000 / 100, version % 100 );
    
		printf ( "CPU��Ƶ��%d MHz\r\n", cpu_clk_freq / 1000000 );  
		
		printf ( "����ж�ʱ�䣺%d us\r\n", 
				ts_int / ( cpu_clk_freq / 1000000 ) ); 

		printf ( "�����������ʱ�䣺%d us\r\n", 
		         OSSchedLockTimeMax / ( cpu_clk_freq / 1000000 ) );		

		printf ( "�����л��ܴ�����%d\r\n", OSTaskCtxSwCtr ); 	
		
		printf ( "CPUʹ���ʣ�%d.%d%%\r\n",
				OSStatTaskCPUUsage / 100, OSStatTaskCPUUsage % 100 );  
		
		printf ( "CPU���ʹ���ʣ�%d.%d%%\r\n", 
		         OSStatTaskCPUUsageMax / 100, OSStatTaskCPUUsageMax % 100 );

		printf ( "���������CPUʹ���ʣ�%d.%d%%\r\n", 
		         USARTTaskTCB.CPUUsage / 100, USARTTaskTCB.CPUUsage % 100 );
						 
		printf ( "���������CPU���ʹ���ʣ�%d.%d%%\r\n", 
		         USARTTaskTCB.CPUUsageMax / 100, USARTTaskTCB.CPUUsageMax % 100 ); 

		printf ( "���������CPUʹ���ʣ�%d.%d%%\r\n", 
		         KEYTaskTCB.CPUUsage / 100, KEYTaskTCB.CPUUsage % 100 );  
						 
		printf ( "���������CPU���ʹ���ʣ�%d.%d%%    \r\n", 
		         KEYTaskTCB.CPUUsageMax / 100, KEYTaskTCB.CPUUsageMax % 100 ); 
		
		printf ( "������������úͿ��ж�ջ��С�ֱ�Ϊ��%d,%d\r\n", 
		         USARTTaskTCB.StkUsed, USARTTaskTCB.StkFree ); 
		
		printf ( "������������úͿ��ж�ջ��С�ֱ�Ϊ��%d,%d\r\n", 
		         KEYTaskTCB.StkUsed, KEYTaskTCB.StkFree ); 
		
		
		OS_CRITICAL_EXIT();                               //�˳��ٽ��
		
	}
}

