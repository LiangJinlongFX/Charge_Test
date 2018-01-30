#ifndef _ELECTRONICLOAD_H
#define _ELECTRONICLOAD_H

#include "sys.h"

/**
 * 
 */
typedef enum 
{
	Load_Outtime		  =(1),		//��ʱ
	Load_Parameter_Error		=(2),		//��������
	Load_OK = 0,						//
}Load_Error;

Load_Error Load_SetCurrent(u16 val,u16 Different_Val,u16 Out_Time); //���õ��Ӹ��غ���ֵ
void Load_GetADC(void);
void LOad_SetConDac(u16 val);

#endif