/* 本硬件端口设置适用于电源适配器检测仪主控板 v 20180318 */

端口设置说明

LED0	PC2
LED1	PC3
LED2	PC4
LED3	PC5

Beep	PC6

DS18B20  PB15


/* ADC端口分配 */
直流电压采集 PB0	ADC1_IN8
纹波电压采集 PA1	ADC1_IN1
直流电流采集 PB1	ADC2_IN9
D-电压采集	 PA6	ADC2_IN6
D+电压采集	 PA7	ADC2_IN7

/* DAC端口分配 */
D-电压 PA4	DAC_OUT1
D+电压 PA5	DAC_OUT2
