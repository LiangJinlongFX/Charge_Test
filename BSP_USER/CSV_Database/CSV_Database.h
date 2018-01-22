#ifndef _FAT_DATABASE_H_
#define _FAT_DATABASE_H_

#include "stm32f4xx.h"
#include "sys.h"

//定义数据文件头部标签 
const char Data_FileHeader[370]= "Serial_Number,Test_Time,Ripple_Voltage,Vout_Max,Cout_Max,"
	"Over_Voltage_Protection,Over_Current_Protection,Short_Current,Quick_Charge,Poweron_Time,Efficiency,Test_Subsequence," ;
const char CommaStr = ',';
const char EnterStr[2] = {".."};


// 测试数据信息结构体
typedef struct 
{
	u16 Serial_Number;		//数据流水号     0 - 65535
	u8	Test_Time[6];			//测试时间（年月日时分秒）  example : 18-01-14-13-45-58
	u8 Ripple_Voltage;			//纹波电压峰值     0 - 255 mV
	u8 Vout_Max;				//最大输出电压     0 - 25.5 V  *10	   
	u8 Cout_Max;				//最大输出电流	  0 - 2.55 A  *100	  
	u8 Over_Voltage_Protection;	//过电压保护		  0 : 不合格/无	1 ： 合格
	u8 Over_Current_Protection;	//过电流保护		  0 : 不合格/无	1 ： 合格
	u8 Short_Current;			//短路保护标志位	  0 : 不合格/无	1 ： 合格
	u8 Quick_Charge;			//快充支持		  0 : 不合格/无	1 ： QC2.0 	2 : QC3.0 	3 : QC4.0 	4 : MTK
	u8 Poweron_Time;			//上电时间		  0 - 255 ms
	u8 Efficiency;				//转换效率		  0 - 100 %
	u8 Test_Subsequence;		//测试结果		  测试结果代码
} TestData_Type;			

typedef enum 
{
	Database_OK=0,
	Database_ERROR,
}Database_Res;

#define TestData_Size 18;   //数据存储结构体大小



u8 Test_ReadData(TestData_Type TestData_Struct);

u8 Test_WriteData(TestData_Type TestData_Struct);

u8 Num_To_Str(u8 number);

u8 Creat_FileHeader(char *File_Name);

u8 Get_DataNum(u8 *path);

int my_atoi(char s[]);

void my_itoa(int n,char s[]);

#endif

