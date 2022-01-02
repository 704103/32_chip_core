#ifndef _sim800c_H
#define _sim800c_H


#include "system.h"


#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8	//�ߵ��ֽڽ����궨��

extern u8 Scan_Wtime;

void sim_send_sms(u8*phonenumber,u8*msg);
void sim_at_response(u8 mode);	
u8* sim800c_check_cmd(u8 *str);
u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 sim800c_chr2hex(u8 chr);
u8 sim800c_hex2chr(u8 hex);
void sim800c_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
void sim800c_load_keyboard(u16 x,u16 y,u8 **kbtbl);
void sim800c_key_staset(u16 x,u16 y,u8 keyx,u8 sta);
u8 sim800c_get_keynum(u16 x,u16 y);
u8 sim800c_call_test(void);			 //���Ų���
void sim800c_sms_read_test(void);	 //�����Ų���
void sim800c_sms_send_test(void);	 //�����Ų��� 
void sim800c_sms_ui(u16 x,u16 y);	 //���Ų���UI���溯��
u8 sim800c_sms_test(void);			 //���Ų���
void sim800c_spp_ui(u16 x,u16 y);    //��������UI���溯��
u8 sim800c_spp_test(void);           //����spp����
u8 sim800c_spp_mode(u8 mode);        //����ģʽѡ��
void sim800c_mtest_ui(u16 x,u16 y);	 //SIM800C������UI
u8 sim800c_gsminfo_show(u16 x,u16 y);//��ʾGSMģ����Ϣ
void ntp_update(void);               //����ͬ��ʱ��
void sim800c_test(void);			 //SIM800C�����Ժ���




#endif
