#include "ft6236.h"
#include "touch.h"
#include "ctiic.h"
#include "usart.h"
#include "SysTick.h" 
#include "string.h" 
#include "led.h" 



u16 ctadjustx=0;
u16 ctadjusty=0;

//��FT5206д��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 FT5206_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();	 
	CT_IIC_Send_Byte(FT_CMD_WR);	//����д���� 	 
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//������
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();					//����һ��ֹͣ����	    
	return ret; 
}
//��FT5206����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���			  
void FT5206_RD_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i; 
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(FT_CMD_WR);   	//����д���� 	 
	CT_IIC_Wait_Ack(); 	 										  		   
 	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(FT_CMD_RD);   	//���Ͷ�����		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //������	  
	} 
    CT_IIC_Stop();//����һ��ֹͣ����     
} 
//��ʼ��FT5206������
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ�� 
u8 FT5206_Init(void)
{
	u8 temp[2];  		
 	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_6);//����
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_7);//����		

	CT_IIC_Init();      	//��ʼ����������I2C����  
	FT_RST=0;				//��λ
	delay_ms(20);
 	FT_RST=1;				//�ͷŸ�λ		    
	delay_ms(50);  	
	temp[0]=0;
	FT5206_WR_Reg(FT_DEVIDE_MODE,temp,1);	//������������ģʽ 
	FT5206_WR_Reg(FT_ID_G_MODE,temp,1);		//��ѯģʽ 
	temp[0]=22;								//������Чֵ��22��ԽСԽ����	
	FT5206_WR_Reg(FT_ID_G_THGROUP,temp,1);	//���ô�����Чֵ
	temp[0]=12;								//�������ڣ�����С��12�����14
	FT5206_WR_Reg(FT_ID_G_PERIODACTIVE,temp,1); 
	//��ȡ�汾�ţ��ο�ֵ��0x3003
	FT5206_RD_Reg(FT_ID_G_LIB_VERSION,&temp[0],2); 	
	
	if(temp[0]==0X30&&temp[1]==0X03)//�汾:0X3003
	{ 
		printf("CTP ID:%x\r\n",((u16)temp[0]<<8)+temp[1]);
		return 0;
	} 
	return 1;
}
const u16 FT5206_TPX_TBL[5]={FT_TP1_REG,FT_TP2_REG,FT_TP3_REG,FT_TP4_REG,FT_TP5_REG};
//ɨ�败����(���ò�ѯ��ʽ)
//mode:0,����ɨ��.
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 FT5206_Scan(u8 mode)
{
	u8 buf[4];
	u8 i=0;
	u8 res=0;
	u8 temp;
	static u8 t=0;//���Ʋ�ѯ���,�Ӷ�����CPUռ����   
	t++;
	if((t%10)==0||t<10)//����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
	{
		FT5206_RD_Reg(FT_REG_NUM_FINGER,&mode,1);//��ȡ�������״̬ 
		printf("mode=%d\r\n",mode);
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);//����ĸ���ת��Ϊ1��λ��,ƥ��tp_dev.sta���� 
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//������Ч?
				{
					FT5206_RD_Reg(FT5206_TPX_TBL[i],buf,4);	//��ȡXY����ֵ 
					tp_dev.x[i]=(((u16)(buf[0]&0X0F)<<8)+buf[1]);
					tp_dev.y[i]=(((u16)(buf[2]&0X0F)<<8)+buf[3]);
					
//					if(tp_dev.touchtype&0X01)//����
//					{
//						tp_dev.y[i]=320-(((u16)(buf[0]&0X0F)<<8)+buf[1]);
//						tp_dev.x[i]=480-(((u16)(buf[2]&0X0F)<<8)+buf[3]);
//					}
//					else
//					{
//						tp_dev.y[i]=480-(((u16)(buf[2]&0X0F)<<8)+buf[3]);
//						tp_dev.x[i]=320-(((u16)(buf[0]&0X0F)<<8)+buf[1]);
//					}  
					if((buf[0]&0XF0)!=0X80)tp_dev.x[i]=tp_dev.y[i]=0;//������contact�¼�������Ϊ��Ч
					printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
				}			
			} 
			res=1;
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)mode=0;	//���������ݶ���0,����Դ˴�����
			t=0;		//����һ��,��������������10��,�Ӷ����������
		}
	}
	if((mode&0X1F)==0)//�޴����㰴��
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);	//��ǰ����ɿ�
		}else						//֮ǰ��û�б�����
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//�������Ч���	
		}	 
	} 	
	if(t>240)t=10;//���´�10��ʼ����
	return res;
}


//��ʾ�ַ���
u8* const TP_REMIND_MSG_TBL1="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 



void CTTouch_Adjust(void)
{
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	double fac; 	
	u16 outtime=0;
 	cnt=0;				
	FRONT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//����   
	FRONT_COLOR=RED;//��ɫ 
	LCD_Clear(WHITE);//���� 	   
	FRONT_COLOR=BLACK;
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL1);//��ʾ��ʾ��Ϣ
	TP_Drow_Touch_Point(20,20,RED);//����1 
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(0);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);				//�����1 
					TP_Drow_Touch_Point(tftlcd_data.width-20,20,RED);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(tftlcd_data.width-20,20,WHITE);	//�����2
					TP_Drow_Touch_Point(20,tftlcd_data.height-20,RED);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,tftlcd_data.height-20,WHITE);			//�����3
 					TP_Drow_Touch_Point(tftlcd_data.width-20,tftlcd_data.height-20,RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					ctadjustx=abs(pos_temp[0][0]-pos_temp[1][0])/2;//x1-x2
					ctadjusty=abs(pos_temp[0][1]-pos_temp[2][1])/2;//y1-y3	
				
					FRONT_COLOR=BLUE;
					LCD_Clear(WHITE);//����
					LCD_ShowString(35,110,tftlcd_data.width,tftlcd_data.height,16,"Touch Screen Adjust OK!");//У�����
					delay_ms(1000);
 					LCD_Clear(WHITE);//����   
					return;//У�����				 
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			
			break;
	 	} 
 	}
}
