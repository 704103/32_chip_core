#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"




int main()
{
	u16 i=0;
	u8 t;
	u8 len;	
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	USART2_Init(115200);
	
	while(1)
	{
		if(USART2_RX_STA&0x8000)
		{					   
			len=USART2_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ: ");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART2, USART2_RX_BUF[t]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			printf("\r\n");//���뻻��
			USART2_RX_STA=0;
		}
		else
		{
			i++;
			if(i%5000==0)
			{
				printf("����PZ6808L-F1������ USART3ͨ��ʵ��\r\n");
				printf("www.prechin.cn\r\n");
			}
			if(i%200==0)
				printf("\r\n����������,�Իس�������\r\n");  
			if(i%20==0)
				led1=!led1;//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
		}
	}
}
