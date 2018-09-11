# USB电源适配器检测仪

**本代码适用于电源适配器检测主控板 V20180318**

## 端口占用说明

| GPIO端口 | 应用     | 备注         |
| -------- | -------- | ------------ |
| PC2      | LED0     |              |
| PC3      | LED1     |              |
| PC4      | LED2     |              |
| PC5      | LED3     |              |
| PC6      | BEEP     |              |
| PB15     | DS18B20  |              |
| PB0      | ADC1_IN8 | 直流电压采集 |
| PA1      | ADC1_IN1 | 纹波电压采集 |
| PB1      | ADC2_IN9 | 直流电流采集 |
| PA6      | ADC2_IN6 | D+电压采集   |
| PA7      | ADC2_IN7 | D-电压采集   |
| PA4      | DAC_OUT1 | D-电压生成   |
| PA5      | DAC_OUT2 | D+电压生成   |

## 代码树

+ CORE
+ SYSTEM
+ BSP_USER
  + led(指示灯驱动)
  + sdio_sdcard(sd卡驱动)
  + dac(dac驱动)
  + CSV_Database(CSV文件读写库)
  + rtc(RTC驱动)
  + HMI(HMI串口屏控制)
  + switch(开关/按键驱动)
  + adc(ADC驱动)
  + spi(SPI驱动)
  + wkup(系统唤醒)
  + Data_Math(数据运算库)
  + DMA(DMA驱动)
  + stmflash()
+ HARDWARE
+ Common
+ STM32F4_LIB
+ FATFS
+ APP
+ DSP
+ USB_OTG
+ USB_MSC
+ USB_APP