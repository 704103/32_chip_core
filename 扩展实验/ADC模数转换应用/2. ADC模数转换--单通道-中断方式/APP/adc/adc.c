#include "adc.h"
#include "SysTick.h"

u16 ADC_ConvertValue=0;

/*******************************************************************************
* �� �� ��         : ADCx_Init
* ��������		   : ADC��ʼ��	
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ADCx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	ADC_InitTypeDef       ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//ADC
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//ģ������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//������ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_NbrOfChannel = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239.5������,��߲���ʱ�������߾�ȷ��			    
	
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);//����ADCת�������ж�
	
	ADC_Cmd(ADC1, ENABLE);//����ADת����
	
	ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬
	
	ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������
	
	// �����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void ADC1_2_IRQHandler(void)
{
	if(ADC_GetITStatus(ADC1,ADC_IT_EOC))
	{
		ADC_ConvertValue=ADC_GetConversionValue(ADC1);
	}
	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
}
