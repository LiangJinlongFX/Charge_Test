#include "hlw8032.h"
#include "sys.h"
#include "usart3.h"
#include <rtthread.h>
#include "math.h" 
#include "arm_math.h"


HLW8032REG_Type HLW8032REG_Structure;	//HLW8032�Ĵ���ֵ�ṹ��
HLW8032Data_Type HLW8032Data_Structure;	//HLW8032�������ݽṹ��


/*
 * �Ӵ��ڸ������ݵ��Ĵ����ṹ�岢У��������Ч��
 * return 0:OK   1:CheckSumERROR
 */
u8 UsartToStruct(void)
{
	u8 CheckSUM=0;
	u8 i;
	
	/* ����У��� */
	for(i=0;i<24;i++)
	{
		if(i!=0&&i!=1&&i!=23)
		{
			CheckSUM+=USART3_RX_BUF[i];
		}
	}
	if(CheckSUM!=USART3_RX_BUF[23]) return 1;	//У��ʹ���
	
	
	HLW8032REG_Structure.StateREG_Val=USART3_RX_BUF[0];
	
	HLW8032REG_Structure.CheckREG_Val=USART3_RX_BUF[1];
	
//ʹ��Ĭ��ֵ
//	HLW8032REG_Structure.VoltageparameterREG_Val=USART3_RX_BUF[2];
//	HLW8032REG_Structure.VoltageparameterREG_Val=(HLW8032REG_Structure.VoltageparameterREG_Val<<8)|USART3_RX_BUF[3];
//	HLW8032REG_Structure.VoltageparameterREG_Val=(HLW8032REG_Structure.VoltageparameterREG_Val<<8)|USART3_RX_BUF[4];
	
	HLW8032REG_Structure.VoltageREG_Val=USART3_RX_BUF[5];
	HLW8032REG_Structure.VoltageREG_Val=(HLW8032REG_Structure.VoltageREG_Val<<8)|USART3_RX_BUF[6];
	HLW8032REG_Structure.VoltageREG_Val=(HLW8032REG_Structure.VoltageREG_Val<<8)|USART3_RX_BUF[7];
	
//ʹ��Ĭ��ֵ
//	HLW8032REG_Structure.CurrentParameterREG_Val=USART3_RX_BUF[8];
//	HLW8032REG_Structure.CurrentParameterREG_Val=(HLW8032REG_Structure.CurrentParameterREG_Val<<8)|USART3_RX_BUF[9];
//	HLW8032REG_Structure.CurrentParameterREG_Val=(HLW8032REG_Structure.CurrentParameterREG_Val<<8)|USART3_RX_BUF[10];

	HLW8032REG_Structure.CurrentREG_Val=USART3_RX_BUF[11];
	HLW8032REG_Structure.CurrentREG_Val=(HLW8032REG_Structure.CurrentREG_Val<<8)|USART3_RX_BUF[12];
	HLW8032REG_Structure.CurrentREG_Val=(HLW8032REG_Structure.CurrentREG_Val<<8)|USART3_RX_BUF[13];

//ʹ��Ĭ��ֵ
//	HLW8032REG_Structure.PowerparameterREG_Val=USART3_RX_BUF[14];
//	HLW8032REG_Structure.PowerparameterREG_Val=(HLW8032REG_Structure.PowerparameterREG_Val<<8)|USART3_RX_BUF[15];
//	HLW8032REG_Structure.PowerparameterREG_Val=(HLW8032REG_Structure.PowerparameterREG_Val<<8)|USART3_RX_BUF[16];

	HLW8032REG_Structure.PowerREG_Val=USART3_RX_BUF[17];
	HLW8032REG_Structure.PowerREG_Val=(HLW8032REG_Structure.PowerREG_Val<<8)|USART3_RX_BUF[18];
	HLW8032REG_Structure.PowerREG_Val=(HLW8032REG_Structure.PowerREG_Val<<8)|USART3_RX_BUF[19];

	HLW8032REG_Structure.DataUpdataREG_Val=USART3_RX_BUF[20];
	
	HLW8032REG_Structure.PFREG_Val=USART3_RX_BUF[21];
	HLW8032REG_Structure.PFREG_Val=(HLW8032REG_Structure.PFREG_Val<<8)|USART3_RX_BUF[22];
	
	HLW8032REG_Structure.CheckSumREG_Val=USART3_RX_BUF[23];
	
	USART3_RX_Size=0;
	
	return 0;
}

/*
 * ��ȡHLW8032��������
 */
u8 HLW8032Get_Data(HLW8032Data_Type* HLW8032Data_Struct)
{
	float temp_Val=0;
	u8 res;
	
	res=UsartToStruct();	//�Ӵ��ڻ�ȡHLW8032�Ĵ���ֵ
	if(res)	return 1;		//У����󷵻�
	if(HLW8032REG_Structure.StateREG_Val==0xaa) return 2;	//оƬ�����������ʧЧ,�Ĵ���ֵ����ʹ��
	
	/* ���ݹ�ʽ����������� */
	HLW8032Data_Struct->AC_Voltage = ((float)default_VoltageparameterREG/(float)HLW8032REG_Structure.VoltageREG_Val)*V_K1;	//�����ѹ
	HLW8032Data_Struct->AC_Current = ((float)default_CurrentParameterREG/(float)HLW8032REG_Structure.CurrentREG_Val)*C_K2;	//�������
	HLW8032Data_Struct->AC_Power   = ((float)default_PowerparameterREG_Val/(float)HLW8032REG_Structure.PowerREG_Val)*P_K3;	//�����й�����

	return 0;
}
