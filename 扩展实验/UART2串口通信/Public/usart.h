#ifndef __usart_H
#define __usart_H

#include "system.h" 
#include "stdio.h" 

//#define USART1_PRINTF
#define USART2_PRINTF
//#define USART3_PRINTF

#define USART2_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define USART3_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	

extern u8  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART3_RX_STA;         		//����״̬���


void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void USART3_Init(u32 bound);


#endif


