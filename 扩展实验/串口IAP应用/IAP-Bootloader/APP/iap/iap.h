#ifndef _iap_H
#define _iap_H

#include "system.h"


typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

#define FLASH_APP1_ADDR		0x08010000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
											//����0X08000000~0X0800FFFF�Ŀռ�ΪIAPʹ��

void iap_load_app(u32 appxaddr);			//ִ��flash/RAM�����app����
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//��ָ����ַ��ʼ,д��bin



#endif
