#ifndef _FAT_DATABASE_H_
#define _FAT_DATABASE_H_

#include "stm32f4xx.h"
#include "sys.h"


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

typedef struct
{
	u8 Ripple_Voltage;			//纹波电压峰值     0 - 255 mV
	u8 Vout_Max;				//最大输出电压     0 - 25.5 V  *10	   
	u8 Cout_Max;				//最大输出电流	  0 - 2.55 A  *100	  
	u8 Over_Voltage_Protection;	//过电压保护		  0 : 不合格/无	1 ： 合格
	u8 Over_Current_Protection;	//过电流保护		  0 : 不合格/无	1 ： 合格
	u8 Short_Current;			//短路保护标志位	  0 : 不合格/无	1 ： 合格
	u8 Quick_Charge;			//快充支持		  0 : 不合格/无	1 ： QC2.0 	2 : QC3.0 	3 : QC4.0 	4 : MTK
	u8 Poweron_Time;			//上电时间		  0 - 255 ms
	u8 Efficiency;				//转换效率		  0 - 100 %
}TestStandard_Type;

/* 实时检测数据结构体 */
typedef struct
{
	u16 V_OUT;
	u16 C_OUT;
	u16 V_D1;
	u16 V_D2;
	u16 V_Ripple;
}ReadTimeData_Type;

/* 测试指标存储结构体 */
typedef struct
{
	u8 Vout_Max;	//最大输出电压
	u8 Cout_Max;	//最大输出电流
	u8 V_Ripple;	//纹波电压
	u8 Poweron_Time;	//上电时间
	u8 Efficiency; //转换效率
	u8 Safety_Code;	//安全检测开关
	u8 Quick_Charge;	//快充检测开关
	u8 Test_Limit;	//检测项目开关
}TestParameters_Type;

typedef enum 
{
	Database_OK=0,
	Database_ERROR,
}Database_Res;

#define TestData_Size 18;   //数据存储结构体大小

extern TestStandard_Type TestStandard_Arrary[4];
extern u8 Current_event;

extern char Global_str[10][10];


u8 Test_ReadData(TestData_Type TestData_Struct);

u8 Test_WriteData(TestData_Type TestData_Struct,char * File_Title);

u8 Num_To_Str(u8 number);

u8 Creat_FileHeader(char *File_Name);

u8 Modify_TestParameters(TestParameters_Type* TestParameters_Structure,u8 Standard_code);

u8 Read_TestParameters(TestParameters_Type* TestParameters_Structure,u8 Standard_code);

u8 Get_DataNum(u8 *path);

u8 Scan_BatchDir(u8 start_val,u8 end_val);

unsigned int my_atoi(char s[]);

void my_itoa(int n,char str[]);

void my_itoa_Dot(int n,char str[],int dot_num);

#endif

