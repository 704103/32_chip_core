#include "iic.h"

extern uchar BUF[8]; 	//���ݻ���

/*******************************************************************************
* ������         : Delay_5us()
* ��������		 : ��ʱ5us
* ����           : ��
* ���         	 : ��
*******************************************************************************/

void Delay_5us()
{
	unsigned char a,b;
	for(b=1;b>0;b--)
		for(a=1;a>0;a--);

}

/**************************************
��ʼ�ź�
**************************************/
void QMC5883_Start()
{
    SDA = 1;  
    SCL = 1; 
    Delay_5us();     
    SDA = 0;    
    Delay_5us();       
    SCL = 0;      
}

/**************************************
ֹͣ�ź�
**************************************/
void QMC5883_Stop()
{
    SDA = 0;                  
    SCL = 1;                  
    Delay_5us();                
    SDA = 1;                  
    Delay_5us();              
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void QMC5883_SendACK(bit ack)
{
    SDA = ack;                
    SCL = 1;                  
    Delay_5us();              
    SCL = 0;                   
    Delay_5us();              
}

/**************************************
����Ӧ���ź�
**************************************/
bit QMC5883_RecvACK()
{
    SCL = 1;                  
    Delay_5us();            
    CY = SDA;                 
    SCL = 0;                  
    Delay_5us();               
    return CY;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void QMC5883_SendByte(uchar dat)
{
    uchar i;

    for (i=0; i<8; i++)         
    {
        dat <<= 1;              //�Ƴ����ݵ����λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay_5us();            //��ʱ
        SCL = 0;                //����ʱ����
        Delay_5us();            //��ʱ
    }
    QMC5883_RecvACK();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
uchar QMC5883_RecvByte()
{
    uchar i;
    uchar dat = 0;

    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        Delay_5us();            //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
        Delay_5us();            //��ʱ
    }
    return dat;
}

//************************д�뵥�ֽ�����***************************

void Single_Write_QMC5883(uchar REG_Address,uchar REG_data)
{
    QMC5883_Start();                  //��ʼ�ź�
    QMC5883_SendByte(Slave_Address);  //�����豸��ַ+д�ź�
    QMC5883_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
    QMC5883_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
    QMC5883_Stop();                   //����ֹͣ�ź�
}

//************************��ȡ���ֽ�����*************************
//uchar Single_Read_QMC5883(uchar REG_Address)
//{   
//	uchar REG_data;
//	QMC5883_Start();                          //��ʼ�ź�
//	QMC5883_SendByte(Slave_Address);           //�����豸��ַ+д�ź�
//	QMC5883_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ	
//	QMC5883_Start();                          //��ʼ�ź�
//	QMC5883_SendByte(Slave_Address+1);         //�����豸��ַ+���ź�
//	REG_data=QMC5883_RecvByte();              //�����Ĵ�������
//	QMC5883_SendACK(1);   
//	QMC5883_Stop();                           //ֹͣ�ź�
//  return REG_data; 
//}

//******************************************************
//��������QMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x00~0x05
//******************************************************
void Multiple_Read_QMC5883(void)
{   
    uchar i;
    QMC5883_Start();                          //��ʼ�ź�
    QMC5883_SendByte(Slave_Address);          //�����豸��ַ+д�ź�
    QMC5883_SendByte(0x00);                   //���ʹ洢��Ԫ��ַ����0x00��ʼ	
    QMC5883_Start();                          //��ʼ�ź�
    QMC5883_SendByte(Slave_Address+1);        //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = QMC5883_RecvByte();          //BUF[0]�洢����
        if (i == 5)
        {
           QMC5883_SendACK(1);                //���һ��������Ҫ�ط�Ӧ��NOACK
        }
        else
        {
          QMC5883_SendACK(0);                 //Ӧ��ACK
       }
   }
    QMC5883_Stop();                           //ֹͣ�ź�
    Delay_ms(5);
}

//��ʼ��QMC5883��������Ҫ��ο�pdf�����޸�****
void Init_QMC5883()
{

	Single_Write_QMC5883(0x09,0x0d);  //���ƼĴ�������
	Single_Write_QMC5883(0x0b,0x01);  //�������ʱ��Ĵ���
	Single_Write_QMC5883(0x20,0x40);  //
	Single_Write_QMC5883(0x21,0x01);  //	
}





























