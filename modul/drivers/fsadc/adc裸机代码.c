#include "adc.h"
#include "exynos_4412.h"

void exynos_adc_init()
{

	ADCCON = ADCCON | (5<<14); /*1. 设置寄存器ADCCON[16]  选择ADC为12位 */
	/*  100 Mhz / 100 = 1Mhz ,0.1us*/
	/*3. 设置寄存器ADCCON[13:6]  设置分频因子为19，  转换速率1MSPS*/
	ADCCON = ADCCON & (~(0XFF << 6)) | (99 <<6); 	/*2. 设置寄存器ADCCON[14]  使能预分频器 */
	ADCCON = ADCCON & (~(1 << 2));  /*4. 设置寄存器ADCCON[2]   设置为正常工作模式 */
	ADCCON = ADCCON & (~(1 << 1));  /*5. 设置寄存器ADCCON[1]   关闭读操作开始转换功能*/
	ADCMUX = ADCMUX & (~0XF) | 0X3; /*6. 设置寄存器ADCMUX[3:0]   选择ADC转换通道*/

}

void exynos_adc_voltage_test()
{
	int adc_val;
	int adcdata;
	exynos_adc_init();
	printf("hello adc voltage!!\n");
	while(1){
		ADCCON = ADCCON | 1;         /*1. 设置寄存器ADCCON[0]   使能开始转换 */
		while(!(ADCCON & (1<<15))){  /*2. 判断寄存器ADCCON[15]  是否转换结束 */
			adcdata = (ADCDAT & 0XFFF);  /*3. 通过寄存器ADCDAT[11:0]  读出ADC的转换结果 */
			adc_val = adcdata * 1800 / 4096; /*4. 通过转换公式将读取的电压值，转换成模拟电压值 */
			printf("adc_val:　%d\n",adc_val); /*5. 将转换的模拟电压值打印输出 */
			printf("voltage:  %d\n",adc_val*10000/4096);
			delay_ms(20);
		}
	}
}




