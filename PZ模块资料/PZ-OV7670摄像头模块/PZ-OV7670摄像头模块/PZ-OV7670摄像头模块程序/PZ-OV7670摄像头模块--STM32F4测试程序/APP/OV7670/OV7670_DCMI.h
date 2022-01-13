#ifndef _OV7670_DCMI_H
#define _OV7670_DCMI_H


#include "system.h"


#define OV7670_VSYNC  	PBin(7)			//ͬ���źż��IO
#define OV7670_WRST		PAout(4)		//дָ�븴λ
#define OV7670_WREN		PGout(9)		//д��FIFOʹ��
#define OV7670_RCK_H	GPIOA->BSRRH=1<<6//GPIO_SetBits(GPIOA,GPIO_Pin_6)////���ö�����ʱ�Ӹߵ�ƽ
#define OV7670_RCK_L	GPIOA->BSRRL=1<<6//GPIO_ResetBits(GPIOA,GPIO_Pin_6)//	//���ö�����ʱ�ӵ͵�ƽ
#define OV7670_RRST		PGout(15)  		//��ָ�븴λ
#define OV7670_CS		PAout(8)  		//Ƭѡ�ź�(OE)


#define OV7670_DATA  ((PEin(6)<<7)|(PEin(5)<<6)|(PBin(6)<<5)|(PCin(11)<<4)|(PCin(9)<<3)|(PCin(8)<<2)|(PCin(7)<<1)|(PCin(6)<<0))   					//��������˿�
/////////////////////////////////////////									
	    				 
u8   OV7670_Init(void);		  	   		 
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(u8 sat);
void OV7670_Brightness(u8 bright);
void OV7670_Contrast(u8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);



#endif
