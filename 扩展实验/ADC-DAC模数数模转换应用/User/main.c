

#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "key.h"
#include "dac.h"
#include "adc.h"

/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main()
{
	u8 i=0;
	u16 value=0;
	float adc_vol;
	u16 dacval;
	float dac_vol;
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	USART1_Init(9600);
	KEY_Init();
	DAC1_Init();
	ADCx_Init();
	
	while(1)
	{
		
		i++;
		if(i%20==0)
		{
			led1=!led1;
		}
		
		if(i%50==0)
		{
			value=Get_ADC_Value(ADC_Channel_1,20);
			printf("ADC1_IN1���ADֵΪ��%d\r\n",value);
			adc_vol=(float)value*(3.3/4096);
			printf("ADC1_IN1����ѹֵΪ��%.2fV\r\n",adc_vol);
			
			DAC_SetChannel1Data(DAC_Align_12b_R,value);
			
			dacval=DAC_GetDataOutputValue(DAC_Channel_1);
			dac_vol=(float)dacval*(3.3/4096);
			printf("���DAC��ѹֵΪ��%.2fV\r\n",dac_vol);
			
			printf("\r\n");
		}
		delay_ms(10);	
	}
}
