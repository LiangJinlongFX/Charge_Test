#ifndef _APP_H
#define _APP_H
/**
  ******************************************************************************
  * @file    STM32F407DEMO/app.h 
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

#include <rtthread.h>

/* ������ƿ� */
static struct rt_mailbox HMI_Response_mb;		//HMI������Ӧ����
static rt_mailbox_t GetData_mb=RT_NULL;					//���ݲɼ�����
static rt_mailbox_t Event_mb=RT_NULL;						//������������

/* ���ڷ��ʼ����ڴ�� */
static char mb_pool[32];
static rt_uint8_t Event_Flag;

static rt_uint8_t Batch_val;	//�������
static rt_uint8_t Standard_val; //���Ա�׼���

static rt_thread_t led0_thread=RT_NULL;
static rt_thread_t led1_thread=RT_NULL;
static rt_thread_t usb_thread=RT_NULL;
static rt_thread_t HMIMonitor_thread=RT_NULL;
static rt_thread_t Master_thread=RT_NULL;
static rt_thread_t CollectData_thread=RT_NULL;
static rt_thread_t HMI_FastTest_thread=RT_NULL;
static rt_thread_t HMI_ShowBatchList_thread=RT_NULL;
static rt_thread_t HMI_SelectBatch_thread=RT_NULL;

void led0_thread_entry(void* parameter);
void usb_thread_entry(void* parameter);
void led1_thread_entry(void* parameter);
void HMIMonitor_thread_entry(void* parameter);
void Master_thread_entry(void* parameter);
void CollectData_thread_entry(void* parameter);
void HMI_FastTest_thread_entry(void* parameter);	//���ټ��HMI�����߳�
void HMI_ShowBatchList_thread_entry(void* parameter); //�����б���ʾ�����߳�
void HMI_SelectBatch_thread_entry(void* parameter);	//����ѡ���߳�
void cpu_usage_idle_hook(void);
void Main_entry(void);

void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor);


#endif

