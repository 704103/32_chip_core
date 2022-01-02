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
#include "vs10xx.h"
#include "mp3player.h"




int main()
{
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	USART1_Init(9600);
	TFTLCD_Init();			//LCD��ʼ��
	KEY_Init();
	EN25QXX_Init();				//��ʼ��EN25Q128	  
	VS_Init();	  				//��ʼ��VS1053
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	FATFS_Init();							//Ϊfatfs��ر��������ڴ�
	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	
	FRONT_COLOR=RED;//��������Ϊ��ɫ 
	while(SD_Init()!=0)
	{	
		LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"SD Card Error!");
	}
					 
	LCD_ShowFont16Char(10, 10, "���пƼ�");
    LCD_ShowFont12Char(10, 30, "www.prechin.cn");
    LCD_ShowFont12Char(10, 50, "���ֲ�����ʵ��");
    LCD_ShowFont12Char(10, 70, "K_UP������+");
    LCD_ShowFont12Char(10, 90, "K_DOWN������-");
    LCD_ShowFont12Char(10, 110, "K_RIGHT����һ��");
	LCD_ShowFont12Char(10, 130, "K_LEFT����һ��");
	
	while(1)
	{
  		led2=0; 	  
		LCD_ShowFont12Char(10,170,"�洢������...");
		printf("Ram Test:0X%04X\r\n",VS_Ram_Test());//��ӡRAM���Խ��	    
		LCD_ShowFont12Char(10,170,"���Ҳ�����..."); 	 	 
 		VS_Sine_Test();	
		LCD_ShowFont12Char(10,170,"��MP3���ֲ���ʵ�顷");		
		led2=1;
		mp3_play();
	} 	   	
	
}
