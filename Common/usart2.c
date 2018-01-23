/**
  ******************************************************************************
  * @file    STM32F407DEMO/usart2.c 
  * @author  Liang
  * @version V1.0.0
  * @date    2018-1-22
  * @brief   
  ******************************************************************************
  * @attention
	* �����rt-thread��֧��
	*	����rt-thread msh�Ĵ�������
  ******************************************************************************
**/


/* include-------------------------------------------------- */
#include "sys.h"
#include "usart2.h"
#include "rtthread.h"

//�Ƿ�ʹ����ϵͳ����
#ifdef	RT_USING_UART

//�Ƿ�ʹ�����豸
#ifdef RT_USING_DEVICE
#include <rtdevice.h>
#endif

#define UART_RX_BUFSZ 8   //���ڽ��ջ����С

/* STM32 uart driver */
struct stm32_uart
{
    struct rt_device parent;
    struct rt_ringbuffer rx_rb;
    USART_TypeDef * uart_base;
    IRQn_Type uart_irq;
    rt_uint8_t rx_buffer[UART_RX_BUFSZ];
    
};

//ʹ�ô���2�����Կ�
#ifdef RT_USING_UART2
struct stm32_uart uart2_device;
#endif


//ϵͳ�������жϺ���
void uart_irq_handler(struct stm32_uart* uart)
{
    /* enter interrupt */
    rt_interrupt_enter();
    
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
    {
				//printf("OK");
        rt_ringbuffer_putchar_force(&(uart->rx_rb), (rt_uint8_t)USART_ReceiveData(uart->uart_base));
        /* invoke callback */
        if(uart->parent.rx_indicate != RT_NULL)
        {
            uart->parent.rx_indicate(&uart->parent, rt_ringbuffer_data_len(&uart->rx_rb));
        }    
    }
    
    /* leave interrupt */
    rt_interrupt_leave();
}


//��ӳ�䴮��2��rt_kprintf
void rt_hw_console_output(const char *str)
{
   /* empty console output */
	
	rt_enter_critical();

	while(*str!='\0')
	{
		if(*str=='\n')
		{
			USART_SendData(USART2, '\r'); 
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		}
		USART_SendData(USART2, *str++); 
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	
	}
	rt_exit_critical();
}

void USART2_printf(char *fmt,...)
{
	char buffer[100];
	u8 i=0;
	u8 ch;
	
	va_list arg_ptr;
	__va_start(arg_ptr,fmt);
	vsnprintf(buffer,100,fmt,arg_ptr);
	while((i<100)&&buffer[i])
	{
		ch=buffer[i++];
		USART2->DR = (u8) ch; 
		while(RESET == USART_GetFlagStatus(USART2,USART_FLAG_TXE));
	}
	__va_end(arg_ptr);
}

//Ӳ�������ڳ�ʼ��
void uart2_init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA9����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA10����ΪUSART2
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2��PA3

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =4;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	
}

void USART2_IRQHandler(void)                	//����2�жϷ������
{
		uart_irq_handler(&uart2_device);
}

//ϵͳ�����ڳ�ʼ��
static rt_err_t rt_uart_init (rt_device_t dev)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;
    
		uart2_init(115200);

    return RT_EOK;
}
//ϵͳ����������
static rt_err_t rt_uart_open(rt_device_t dev, rt_uint16_t oflag)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Enable the UART Interrupt */
        NVIC_EnableIRQ(uart->uart_irq);
    }

    return RT_EOK;
}
//ϵͳ���رմ����ж�
static rt_err_t rt_uart_close(rt_device_t dev)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Disable the UART Interrupt */
        NVIC_DisableIRQ(uart->uart_irq);
    }

    return RT_EOK;
}
//ϵͳ�����ڶ�����
static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_size_t length;
    struct stm32_uart* uart;
    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)dev;
    /* interrupt receive */
    rt_base_t level;

    RT_ASSERT(uart != RT_NULL);

    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    length = rt_ringbuffer_get(&(uart->rx_rb), buffer, size);
    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return length;
}
//ϵͳ������д����
static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    char *ptr = (char*) buffer;
    struct stm32_uart* uart;
    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->open_flag & RT_DEVICE_FLAG_STREAM)
    {
        /* stream mode */
        while (size)
        {
            if (*ptr == '\n')
            {
                while(RESET == USART_GetFlagStatus(uart->uart_base,USART_FLAG_TXE));
                USART_SendData(uart->uart_base, '\r');   
            }

            while(RESET == USART_GetFlagStatus(uart->uart_base,USART_FLAG_TXE));
            USART_SendData(uart->uart_base, *ptr); 

            ptr ++;
            size --;
        }
    }
    else
    {
        while (size)
        {
            while(RESET == USART_GetFlagStatus(uart->uart_base,USART_FLAG_TXE));
            USART_SendData(uart->uart_base, *ptr); 

            ptr++;
            size--;
        }
    }

    return (rt_size_t) ptr - (rt_size_t) buffer;
}

/*
	ϵͳ�����ڳ�ʼ������
	ʹ�ô���2
*/
int rt_hw_usart_init(void)
{
		struct stm32_uart* uart;

		/* get uart device */
		uart = &uart2_device;

		/* device initialization */
		uart->parent.type = RT_Device_Class_Char;
		uart->uart_base = USART2;
		uart->uart_irq = USART2_IRQn;
		rt_ringbuffer_init(&(uart->rx_rb), uart->rx_buffer, sizeof(uart->rx_buffer));

		/* device interface */
		uart->parent.init 	    = rt_uart_init;
		uart->parent.open 	    = rt_uart_open;
		uart->parent.close      = rt_uart_close;
		uart->parent.read 	    = rt_uart_read;
		uart->parent.write      = rt_uart_write;
		uart->parent.control    = RT_NULL;
		uart->parent.user_data  = RT_NULL;

		rt_device_register(&uart->parent, "uart2", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    return 0;	
}
INIT_BOARD_EXPORT(rt_hw_usart_init);

#endif /*RT_USING_UART*/

	
