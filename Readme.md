# USB电源适配器检测仪

**本代码适用于电源适配器检测主控板 V20180318**

## GPIO分配说明

| GPIO端口   | 应用             | 备注           |
| ---------- | ---------------- | -------------- |
| PA0        | 唤醒控制         |                |
| PA1        | ADC通道1         | 纹波检测       |
| PA2--PA3   | 串口2接口        |                |
| PA4        | DAC输出通道1     | USB D-电压生成 |
| PA5        | DAC输出通道2     | USB D+电压生成 |
| PA6        | ADC通道6         | USB D+电压检测 |
| PA7        | ADC通道7         | USB D-电压检测 |
| PA8        | DS18B20          |                |
| PA9--PA10  | 串口1接口        |                |
| PA11--PA12 | USB接口          |                |
| PA13--PA14 | SWD接口          |                |
| PA15       | 散热风扇控制     |                |
| PB0        | ADC通道8         | USB 电压检测   |
| PB1        | ADC通道9         | USB 电流检测   |
| PB2        | BOOT1            |                |
| PB3--PB6   | SPI3接口         |                |
| PB7        | 控制面板备用接口 |                |
| PB10--PB11 | 串口3接口        |                |
| PB12--PB15 | 备用接口         |                |
| PC0        | SW               | 负载开关       |
| PC1        | POWERON          | 过流触发引脚   |
| PC2        | LED0             |                |
| PC3        | LED1             |                |
| PC4        | LED2             |                |
| PC5        | LED3             |                |
| PC6        | BEEP             |                |
| PC7        | TEST             | 测试启停按钮   |
| PC8--PC12  | SDIO接口         |                |
| PC13       | 交流接入继电器   |                |

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