#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "key.h"
#include "malloc.h" 
#include "sd.h"
#include "flash.h"
#include "ff.h" 
#include "fatfs_app.h"
#include "key.h"
#include "font_show.h"
#include "touch.h"
#include "usart2.h"
#include "sim800c.h"



int main()
{
	u8 key;
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	KEY_Init();
	USART1_Init(9600);
	TFTLCD_Init();	//LCD��ʼ��
	EN25QXX_Init();	//��ʼ��EN25Q128	 
	tp_dev.init();			//��ʼ��������			 
	USART2_Init(115200);
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	FATFS_Init();							//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 					//����SD�� 
 	f_mount(fs[1],"1:",1); 				//����FLASH.
	key=KEY_Scan(1);
	if(key==KEY_UP)
	{
		LCD_Clear(WHITE);	                       //����
		TP_Adjust();  		                       //��ĻУ׼ 
		TP_Save_Adjdata();	  
		LCD_Clear(WHITE);   	                   //����
	}
	EN25QXX_Init();	//��ʼ��EN25Q128	
	FRONT_COLOR=RED;//��������Ϊ��ɫ 
	sim800c_test();                                //GSM����
	
}
