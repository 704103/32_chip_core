#ifndef _font_show_H
#define _font_show_H


#include "system.h"


/* �ֿ��ļ���ַ */
#define GUI_ASCII_FILE  "ϵͳ/FONT/ASCII��8X16��.DZK"
#define GUI_12CHAR_FILE "ϵͳ/FONT/12�����壨16X16��.DZK"
#define GUI_16CHAR_FILE "ϵͳ/FONT/16�����壨24X21��.DZK"


/* �����ֿ��ַ */
#define GUI_FLASH_ASCII_ADDR     14501454  //ASCII�ֿ��׵�ַ��14502912 - 1456 - 2��
#define GUI_FLASH_12CHAR_ADDR    14502912  //12���ֿ��׵�ַ��15268994 - 766080 - 2��
#define GUI_FLASH_16CHAR_ADDR    15268994  //16���ֿ��׵�ַ��16777216 - 1508220 - 2��

/* �����ֿ�ѡ���� */
#define GUI_UPDATE_ASCII         0x01     
#define GUI_UPDATE_12CHAR        0x02
#define GUI_UPDATE_16CHAR        0x04
#define GUI_UPDATE_ALL           0x07


void FontUpdate(uint8_t updateState);
void LCD_ShowFont12Char(uint16_t x, uint16_t y, uint8_t *ch);
void LCD_ShowFont16Char(uint16_t x, uint16_t y, uint8_t *ch);




#endif
