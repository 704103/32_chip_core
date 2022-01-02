#include "usart.h"		 

/*******************************************************************************
* �� �� ��         : USART5_Init
* ��������		   : USART5��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/ 
void UART5_Init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
 
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;//TX			  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOC,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//RX			 //��������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOD,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

   //USART5 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(UART5, USART_FLAG_TC);
		
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart15 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

/*******************************************************************************
* �� �� ��         : USART5_IRQHandler
* ��������		   : USART5�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void UART5_IRQHandler(void)                	//����1�жϷ������
{
	u8 r;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�
	{
		r =USART_ReceiveData(UART5);//(USART1->DR);	//��ȡ���յ�������
		USART_SendData(UART5,r);
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC) != SET);
	} 
	USART_ClearFlag(UART5,USART_FLAG_TC);
} 	

 



