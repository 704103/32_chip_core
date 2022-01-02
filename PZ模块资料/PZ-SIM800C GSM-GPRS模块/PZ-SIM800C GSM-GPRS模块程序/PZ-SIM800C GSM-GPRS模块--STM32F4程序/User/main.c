#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h" 
#include "touch.h"
#include "malloc.h" 
#include "sdio_sdcard.h" 
#include "flash.h"
#include "ff.h" 
#include "fatfs_app.h"
#include "key.h"
#include "font_show.h"
#include "touch.h"		
#include "usart3.h"
#include "sim800c.h"


int main()
{	
	u8 key;
	
	SysTick_Init(168);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	KEY_Init();
	USART1_Init(9600);
	TFTLCD_Init();	//LCD��ʼ��
	EN25QXX_Init();	//��ʼ��EN25Q128	 
	TOUCH_Init();			//��ʼ��������			 
	USART3_Init(115200);
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMCCM);		                   //��ʼ��CCM�ڴ��
	FATFS_Init();							//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 					//����SD�� 
 	f_mount(fs[1],"1:",1); 				//����FLASH.
	key=KEY_Scan(1);
	if(key==KEY_UP)
	{
		LCD_Clear(WHITE);	                       //����
		TOUCH_Adjust();  		                       //��ĻУ׼ 
		LCD_Clear(WHITE);   	                   //����
	}
	EN25QXX_Init();	//��ʼ��EN25Q128	
	FRONT_COLOR=RED;//��������Ϊ��ɫ 
	sim800c_test();                                //GSM����
}


