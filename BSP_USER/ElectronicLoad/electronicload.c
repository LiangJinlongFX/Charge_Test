#include "sys.h"
#include "electronicload.h"


/**
 * ���Ӹ��غ���
 * @param  val Different_Val [����ֵ +-���ֵ ��ʱ���� ]
 * @return         [description]
 */
Load_Error Load_SetCurrent(u16 val,u16 Different_Val,u16 Out_Time)
{
	u16 Val_Max,Val_Min;
	u16 Current_Val;
	u16 Contr_Val;
	u16 i=0;
	//���÷�Χֵ
	Val_Max=val+Different_Val;
	if(val-Different_Val>1)
		Val_Min=val-Different_Val;
	//������ֵת���ɿ��Ƶ�ѹֵ
	//�ͷſ��Ƶ�ѹ�Ե��Ӹ��ص��ڵ���
	while
	{
		//���ÿ��Ƶ�ѹ
		Load_SetConDac();
		//��ȡ����ֵ
		//�ж��Ƿ����㷶Χ
		if(Current_Val>=Val_Min&&Current_Val<=Val_Max)
			break;
		//����
		else if(Current_Val>Val_Max) Contr_Val-=100;
		//С��
		else if(Current_Val<Val_Min) Contr_Val+=100;
		//��ʱ�˳�
		if(i>Out_Time)
			return Load_Outtime;
		i++;
	}
	
	return Load_OK;
}



