

#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "tftlcd.h"
#include "hc05.h"
#include "usart2.h"			 	 
#include "string.h"

	

//��ʾHC05ģ�������״̬
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(10,140,200,16,16,"ROLE:Master");	//����
	else LCD_ShowString(10,140,200,16,16,"ROLE:Slave ");			 		//�ӻ�
}

//��ʾHC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	if(HC05_STATE)LCD_ShowString(110,140,120,16,16,"STA:Connected ");			//���ӳɹ�
	else LCD_ShowString(110,140,120,16,16,"STA:Disconnect");	 			//δ����				 
}


int main()
{
	u8 t=0;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0; 
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	KEY_Init();
	USART1_Init(9600);
	TFTLCD_Init();			//LCD��ʼ��
	
	
	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"PRECHIN");
	LCD_ShowString(10,30,tftlcd_data.width,tftlcd_data.height,16,"www.prechin.com");
	LCD_ShowString(10,50,tftlcd_data.width,tftlcd_data.height,16,"BT05 BlueTooth Test");
	delay_ms(1000);			//�ȴ�����ģ���ϵ��ȶ�
	
	while(HC05_Init()) 		//��ʼ��HC05ģ��  
	{
		LCD_ShowString(10,90,200,16,16,"HC05 Error!    "); 
		delay_ms(500);
		LCD_ShowString(10,90,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	 										   	   
	LCD_ShowString(10,90,210,16,16,"K_UP:ROLE K_DOWN:SEND/STOP");  
	LCD_ShowString(10,110,200,16,16,"HC05 Standby!");  
  	LCD_ShowString(10,160,200,16,16,"Send:");	
	LCD_ShowString(10,180,200,16,16,"Receive:"); 
	
	FRONT_COLOR=BLUE;
	HC05_Role_Show();
	delay_ms(100);
	USART2_RX_STA=0;
 	while(1) 
	{		
		key=KEY_Scan(0);
		if(key==KEY_UP)						//�л�ģ����������
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//״̬ȡ��	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//��λHC05ģ��
				delay_ms(200);
			}
		}
		else if(key==KEY_DOWN)
		{
			sendmask=!sendmask;				//����/ֹͣ����  	 
			if(sendmask==0)LCD_Fill(10+40,160,240,160+16,WHITE);//�����ʾ
		}
		else delay_ms(10);	   
		if(t==50)
		{
			if(sendmask)					//��ʱ����
			{
				sprintf((char*)sendbuf,"PREHICN HC05 %d\r\n",sendcnt);
	  			LCD_ShowString(10+40,160,200,16,16,sendbuf);	//��ʾ��������	
				usart2_printf("PREHICN HC05 %d\r\n",sendcnt);	//���͵�����ģ��
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
			led1=!led1; 	     
		}	  
		if(USART2_RX_STA&0X8000)			//���յ�һ��������
		{
			LCD_Fill(10,200,240,320,WHITE);	//�����ʾ
 			reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
		  	USART2_RX_BUF[reclen]='\0';	 	//���������
			printf("reclen=%d\r\n",reclen);
			printf("USART2_RX_BUF=%s\r\n",USART2_RX_BUF);
			if(reclen==10||reclen==11) 		//����D2���
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED2 ON\r\n")==0)led2=0;//��LED2
				if(strcmp((const char*)USART2_RX_BUF,"+LED2 OFF\r\n")==0)led2=1;//�ر�LED2
			}
 			LCD_ShowString(10,200,209,119,16,USART2_RX_BUF);//��ʾ���յ�������
 			USART2_RX_STA=0;	 
		}	 															     				   
		t++;	
	}
}
