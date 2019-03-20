#ifndef _CSV_DATABASE_H_
#define _CSV_DATABASE_H_

#include "stm32f4xx.h"
#include "sys.h"


/* 测试数据信息结构体 */
typedef struct 
{
	u16 Serial_Number;					//数据流水号     0 - 65535
	u8	Test_Time[6];						//测试时间（年月日时分秒）  example : 18-01-14 13:45:58
	float Vout_Max;							//最大输出电压   0 - 30.000V 
	float Vout_Min;							//最小输出电压   0 - 30.000V 
	float Cout_Max;							//最大输出电流	 0 - 3.000A
	float Cout_short;						//短路电流			 0 - 1.000A
	float	Noload_Power;					//空载功率
	float Efficiency;						//转换效率		  0 - 100 %
	float Ripple_Voltage;				//纹波电压峰值   0 - 1000 mV	
	u8 Over_Current_Protection;	//过电流保护		  0 : 不合格/无	1 ： 合格
	u8 Quick_Charge;						//快充支持		  0 : 不合格/无	1 ： QC2.0 	2 : QC3.0 	3 : QC4.0 	4 : MTK
	u8 Test_Subsequence;				//测试结果		  测试结果代码
}TestData_Type;

/**
 * 实时检测数据结构体
 * @brief 用于检测过程中缓存实时检测数据 
 */
typedef struct
{
	float V_IN;
	float C_IN;
	float V_OUT;
	float C_OUT;
	float V_D1;
	float V_D2;
	float	V_Ripple;
}ReadTimeData_Type;

/**
 * 测试指标存储结构体
 * 额定输出电压		 [一般情况下的输出电压(非快充情况)]
 * 最大输出电流		 [引起过流保护前的最大输出电流]
 * 输出电压容差		 [允许额定输出电压在+/-%浮动]
 * 纹波电压				 [允许最大的电源纹波峰值]
 * 转换效率				 [AC/DC转换效率]
 * 安全检测位标志  bit0:过压保护		bit1:短路保护		bit2:过流保护 [ 1-生效	目前仅支持过流保护测试]
 * 快充诱导标志位	 bit0:MTK		bit1:QC2.0		bit2:QC3.0	[	1-生效]
 * 检测项目开关		 [检测项目开关,当前指标下需检测的项目 1-生效]
 **/
typedef struct
{
	u16 Vout;						//额定输出电压  [x]mV
	u16 Cout_Max;				//最大输出电流	[x]mA
	u16 Vout_Tolerance;	//输出电压容差	+-[X]mV
	u16 V_Ripple;				//纹波电压			[x]mV
	u16 Efficiency; 		//转换效率			未定义
	u16 Safety_Code;		//安全检测开关	bit0: OCP
	u16 Quick_Charge;		//快充检测开关	诱导设置说明 |不诱导 Detected_level=0|仅检测是否有快充功能 Detected_level=0x0E|检测MTK-PE并检测诱导出具体的电压值 Detected_level=0x09
	u16 Test_Limit;			//检测项目开关	未定义,使用全局检测开关
}TestParameters_Type;

/* 数据库函数错误码 */
typedef enum 
{
	Database_OK=0,
	Database_ERROR,
}Database_Res;

/* 文件标题大小 !!!需正确配置大小 */
#define FileHeader_Size 181	

extern u8 Current_event;

extern char Global_str[30][20];


u8 Test_ReadData(TestData_Type TestData_Struct);																					//读取特定文件下的测试数据条目
u8 Test_WriteData(TestData_Type TestData_Struct,char* BatchName);													//写入特定批量下的测试数据条目
u8 Creat_FileHeader(char *File_Name);																											//创建测试文件并配置文件头
u8 Modify_TestParameters(TestParameters_Type* TestParameters_Structure,u8 Standard_code);	//写入测试配置数据
u8 First_writeTestParameters(void);																												//创建测试配置文件
u8 Read_TestParameters(TestParameters_Type* TestParameters_Structure,u8 Standard_code);		//读取测试配置数据
u8 Creat_NewBatchDir(char* path);																													//创建新的批量目录
u8 Scan_BatchDir(u8 start_val,u8 end_val,char *DirStr[]);																		//检索批量目录
u8 Poweron_ReadTestParameters(void); 																											//上电时读出SD卡中配置文件

unsigned int my_atoi(char s[]);
void my_itoa(int n,char str[]);
void my_itoa_Dot(int n,char str[],int dot_num);
void my_strncpy(char* str1,char* str2,u8 start_addr,u8 n);



#endif

