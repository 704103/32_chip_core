#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "key.h"
#include "touch.h"

//�����Ļ�������Ͻ���ʾ"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	FRONT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(tftlcd_data.width-24,0,200,16,16,"RST");//��ʾ��������
  	FRONT_COLOR=RED;//���û�����ɫ 
}

//��һ�����(2*2�ĵ�)		   
//x,y:����
//color:��ɫ
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	FRONT_COLOR=color;
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}
////////////////////////////////////////////////////////////////////////////////
//���ݴ�����ר�в���
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

//5�����ص����ɫ(���ݴ�������)												 
const u16 POINT_COLOR_TBL[5]={RED,GREEN,BLUE,BROWN,GRED}; 

//���ݴ��������Ժ���
void ctp_test(void)
{
	u8 t=0;
	u8 i=0;	  	    
 	u16 lastpos[5][2];		//���һ�ε����� 
	while(1)
	{
		TOUCH_Scan();
		for(t=0;t<5;t++)
		{	
			if(TouchData.tpsta&(1<<t))
			{
				if(TouchData.lcdx[t]<tftlcd_data.width&&TouchData.lcdy[t]<tftlcd_data.height)
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = TouchData.lcdx[t];
						lastpos[t][1] = TouchData.lcdy[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],TouchData.lcdx[t],TouchData.lcdy[t],2,POINT_COLOR_TBL[t]);//����
					lastpos[t][0]=TouchData.lcdx[t];
					lastpos[t][1]=TouchData.lcdy[t];
					if(TouchData.lcdx[t]>(tftlcd_data.width-24)&&TouchData.lcdy[t]<20)
					{
						Load_Drow_Dialog();//���
					}
				}
			}
			else lastpos[t][0]=0XFFFF;
		}
		delay_ms(5);i++;
		if(i%20==0)led1=!led1;
	}	
}

//���败�������Ժ���
void rtp_test(void)
{
	u8 key;
	u8 i=0;	  
	while(1)
	{
	 	key=KEY_Scan(0);
		if(TOUCH_Scan()==0)
		{
			if(TouchData.lcdx[0]<tftlcd_data.width&&TouchData.lcdy[0]<tftlcd_data.height)
			{	
				if(TouchData.lcdx[0]>(tftlcd_data.width-24)&&TouchData.lcdy[0]<16)Load_Drow_Dialog();//���
				else TP_Draw_Big_Point(TouchData.lcdx[0],TouchData.lcdy[0],RED);		//��ͼ	  			   
			}
			else delay_ms(10);	//û�а������µ�ʱ��
		}			
		 	    
		if(key==KEY_UP)	//KEY_UP����,��ִ��У׼����
		{
			TOUCH_Adjust(); //��ĻУ׼  
			Load_Drow_Dialog();
		}
		i++;
		if(i%20==0)led1=!led1;
	}
}

int main()
{	
	u8 i=0;
	
	SysTick_Init(168);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	USART1_Init(9600);
	TFTLCD_Init();			//LCD��ʼ��
	KEY_Init();
	TOUCH_Init();
	FRONT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(10,10,200,16,16,"PRECHIN STM32F4");	
	LCD_ShowString(10,30,200,16,16,"TOUCH Test");	
	LCD_ShowString(10,50,200,16,16,"www.prechin.cn");
	LCD_ShowString(10,70,200,16,16,"Press K_UP to Adjust");
	delay_ms(1500);
 	Load_Drow_Dialog();
	while(1)
	{
		#ifdef TFTLCD_HX8352C
			rtp_test();
		#endif
		
		#ifdef TFTLCD_ILI9488
			ctp_test();
		#endif
		
		#ifdef TFTLCD_NT35510
			ctp_test();
		#endif
		
	}
}


