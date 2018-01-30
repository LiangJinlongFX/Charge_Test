#include "sys.h"
#include "electronicload.h"


/**
 * 电子负载恒流
 * @param  val Different_Val [电流值 +-误差值 超时次数 ]
 * @return         [description]
 */
Load_Error Load_SetCurrent(u16 val,u16 Different_Val,u16 Out_Time)
{
	u16 Val_Max,Val_Min;
	u16 Current_Val;
	u16 Contr_Val;
	u16 i=0;
	//设置范围值
	Val_Max=val+Different_Val;
	if(val-Different_Val>1)
		Val_Min=val-Different_Val;
	//将电流值转换成控制电压值
	//释放控制电压以电子负载调节电流
	while
	{
		//设置控制电压
		Load_SetConDac();
		//读取电流值
		//判断是否满足范围
		if(Current_Val>=Val_Min&&Current_Val<=Val_Max)
			break;
		//大了
		else if(Current_Val>Val_Max) Contr_Val-=100;
		//小了
		else if(Current_Val<Val_Min) Contr_Val+=100;
		//超时退出
		if(i>Out_Time)
			return Load_Outtime;
		i++;
	}
	
	return Load_OK;
}



