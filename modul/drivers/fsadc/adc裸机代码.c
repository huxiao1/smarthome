#include "adc.h"
#include "exynos_4412.h"

void exynos_adc_init()
{

	ADCCON = ADCCON | (5<<14); /*1. ���üĴ���ADCCON[16]  ѡ��ADCΪ12λ */
	/*  100 Mhz / 100 = 1Mhz ,0.1us*/
	/*3. ���üĴ���ADCCON[13:6]  ���÷�Ƶ����Ϊ19��  ת������1MSPS*/
	ADCCON = ADCCON & (~(0XFF << 6)) | (99 <<6); 	/*2. ���üĴ���ADCCON[14]  ʹ��Ԥ��Ƶ�� */
	ADCCON = ADCCON & (~(1 << 2));  /*4. ���üĴ���ADCCON[2]   ����Ϊ��������ģʽ */
	ADCCON = ADCCON & (~(1 << 1));  /*5. ���üĴ���ADCCON[1]   �رն�������ʼת������*/
	ADCMUX = ADCMUX & (~0XF) | 0X3; /*6. ���üĴ���ADCMUX[3:0]   ѡ��ADCת��ͨ��*/

}

void exynos_adc_voltage_test()
{
	int adc_val;
	int adcdata;
	exynos_adc_init();
	printf("hello adc voltage!!\n");
	while(1){
		ADCCON = ADCCON | 1;         /*1. ���üĴ���ADCCON[0]   ʹ�ܿ�ʼת�� */
		while(!(ADCCON & (1<<15))){  /*2. �жϼĴ���ADCCON[15]  �Ƿ�ת������ */
			adcdata = (ADCDAT & 0XFFF);  /*3. ͨ���Ĵ���ADCDAT[11:0]  ����ADC��ת����� */
			adc_val = adcdata * 1800 / 4096; /*4. ͨ��ת����ʽ����ȡ�ĵ�ѹֵ��ת����ģ���ѹֵ */
			printf("adc_val:��%d\n",adc_val); /*5. ��ת����ģ���ѹֵ��ӡ��� */
			printf("voltage:  %d\n",adc_val*10000/4096);
			delay_ms(20);
		}
	}
}




