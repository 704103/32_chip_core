#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "key.h"
#include "stm32_flash.h"
#include "iap.h"




int main()
{
	u8 t;
	u8 key;
	u16 oldcount=0;				//�ϵĴ��ڽ�������ֵ
	u16 applenth=0;				//���յ���app���볤��
	u8 clearflag=0;
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	USART1_Init(115200);
	TFTLCD_Init();			//LCD��ʼ��
	KEY_Init();
	
	FRONT_COLOR=BLACK;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"PRECHIN STM32F1");
	LCD_ShowString(10,30,tftlcd_data.width,tftlcd_data.height,16,"www.prechin.net");
	LCD_ShowString(10,50,tftlcd_data.width,tftlcd_data.height,16,"IAP Test");
	LCD_ShowString(10,70,tftlcd_data.width,tftlcd_data.height,16,"K_UP:Copy APP2FLASH");
	LCD_ShowString(10,90,tftlcd_data.width,tftlcd_data.height,16,"K_DOWN:Erase SRAM APP");
	LCD_ShowString(10,110,tftlcd_data.width,tftlcd_data.height,16,"K_LEFT:Run FLASH APP");
	LCD_ShowString(10,130,tftlcd_data.width,tftlcd_data.height,16,"K_RIGHT:Run SRAM APP");
	
	FRONT_COLOR=RED;
	
	
	while(1)
	{
		if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",applenth);
			}else oldcount=USART_RX_CNT;			
		}
		t++;
		delay_ms(10);
		if(t==30)
		{
			led1=!led1;
			t=0;
			if(clearflag)
			{
				clearflag--;
				if(clearflag==0)LCD_Fill(10,210,240,210+16,WHITE);//�����ʾ
			}
		}	  	 
		key=KEY_Scan(0);
		if(key==KEY_UP)
		{
			if(applenth)
			{
				printf("��ʼ���¹̼�...\r\n");	
				LCD_ShowString(10,210,200,16,16,"Copying APP2FLASH...");
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����   
					LCD_ShowString(10,210,200,16,16,"Copy APP Successed!!");
					printf("�̼��������!\r\n");	
				}else 
				{
					LCD_ShowString(10,210,200,16,16,"Illegal FLASH APP!  ");	   
					printf("��FLASHӦ�ó���!\r\n");
				}
 			}else 
			{
				printf("û�п��Ը��µĹ̼�!\r\n");
				LCD_ShowString(10,210,200,16,16,"No APP!");
			}
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ									 
		}
		if(key==KEY_DOWN)
		{
			if(applenth)
			{																	 
				printf("�̼�������!\r\n");    
				LCD_ShowString(10,210,200,16,16,"APP Erase Successed!");
				applenth=0;
			}else 
			{
				printf("û�п�������Ĺ̼�!\r\n");
				LCD_ShowString(10,210,200,16,16,"No APP!");
			}
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ									 
		}
		if(key==KEY_LEFT)
		{
			printf("��ʼִ��FLASH�û�����!!\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}else 
			{
				printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
				LCD_ShowString(10,210,200,16,16,"Illegal FLASH APP!");	   
			}									 
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ	  
		}
		if(key==KEY_RIGHT)
		{
			printf("��ʼִ��SRAM�û�����!!\r\n");
			if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//�ж��Ƿ�Ϊ0X20XXXXXX.
			{	 
				iap_load_app(0X20001000);//SRAM��ַ
			}else 
			{
				printf("��SRAMӦ�ó���,�޷�ִ��!\r\n");
				LCD_ShowString(10,210,200,16,16,"Illegal SRAM APP!");	   
			}									 
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ	 
		}
			
	}
}
