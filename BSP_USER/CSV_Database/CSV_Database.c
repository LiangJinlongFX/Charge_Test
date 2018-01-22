#include "CSV_Database.h"
// 使用itoa和atoi函数加入
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "string.h"
#include "ff.h"
#include "sys.h"
#include "stm32f4xx.h"






/**
 * 测试数据信息存储到文件系统
 * @param  DataStruct [description]
 * @return            [description]
 */
u8 Write_Data(TestData_Type DataStruct)
{
	u8 res;
	FIL 	Fsrc;									//定义文件对象
	DIR   	Dir;									//定义目录对象
	u16 	count=0;								//定义统计数
	u16		count_CRC=0;																		
	u32   	REsize;
	char 	File_Name[12];							//定义文件名数组
	char  	Dir_Name[6];							//定义目录名数组
	char 	countStr[5];
	char 	TempStr[6];
	UINT 	check_count;							//定义检查统计数
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	//流水号默认为0
	DataStruct.Serial_Number=0;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);   	//获取当前日期
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);		//获取当前时间
	//往数据结构体写入检测时间
	DataStruct.Test_Time[0]=RTC_DateStruct.RTC_Year;
	DataStruct.Test_Time[1]=RTC_DateStruct.RTC_Month;
	DataStruct.Test_Time[2]=RTC_DateStruct.RTC_Date;
	DataStruct.Test_Time[3]=RTC_TimeStruct.RTC_Hours;
	DataStruct.Test_Time[4]=RTC_TimeStruct.RTC_Minutes;
	DataStruct.Test_Time[5]=RTC_TimeStruct.RTC_Seconds;

	//生成目录名
	sprintf(Dir_Name,"0:%02d%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month);
	//生成文件名
	sprintf(File_Name,"0:%02d%02d%02d.csv",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	//打开目录
	res=f_opendir(&Dir,(const TCHAR*)Dir_Name);
	if(res==FR_NO_PATH)
	{
		res=f_mkdir((const TCHAR*)Dir_Name);
		if(res!=0) return 1;
		res=f_opendir(&Dir,(const TCHAR*)Dir_Name);
		if(res!=0) return 1;
	}
	else if(res!=0) return 1;

	//打开文件
	res=f_open(&Fsrc,File_Name,FA_READ|FA_WRITE);
	// 如果文件并不存在的打开错误则创建文件
	if(res!=0)
	{ 
		if(Creat_FileHeader(File_Name)!=0)
			return 1;
	}
	// 如果其他未知错误则结束
	else return 1;
	//读取文件数据总量并转换为数值(偏移量：371 字节数：5)
	res=f_lseek(&Fsrc,371);
	if(res!=0) return 1;
	// 读出数据
	res=f_read(&Fsrc,&countStr,5,&check_count);
	if(res!=0||check_count!=5) return 1; 
	// 转换格式
	count=my_atoi(countStr);
	// 根据总数给数据体写入序列号
	DataStruct.Serial_Number=count+1;		
	//预估文件空间
	REsize=378+(count+1)*sizeof(DataStruct);
	//如果文件空间不足则扩展文件空间
	if(Fsrc.fsize<REsize)
	{
		res=f_lseek(&Fsrc,REsize);
		if(res!=0) return 1;
	}

	//序列号转换为字符串
	my_itoa(DataStruct.Serial_Number,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,5,&check_count);
	if(res!=0||check_count!=5)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//年份转换为字符串
	my_itoa(DataStruct.Test_Time[0],TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+5);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	//月份转换为字符串
	my_itoa(DataStruct.Test_Time[1],TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+7);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	//日转换为字符串
	my_itoa(DataStruct.Test_Time[2],TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	//时转换为字符串
	my_itoa(DataStruct.Test_Time[3],TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	//分转换为字符串
	my_itoa(DataStruct.Test_Time[4],TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	//秒转换为字符串
	my_itoa(DataStruct.Test_Time[5],TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//纹波电压转换为字符串
	my_itoa(DataStruct.Ripple_Voltage,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//最大输出电压转换为字符串
	my_itoa(DataStruct.Vout_Max,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//最大输出电流转换为字符串
	my_itoa(DataStruct.Cout_Max,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//过压保护转换为字符串
	my_itoa(DataStruct.Over_Voltage_Protection,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//过流保护转换为字符串
	my_itoa(DataStruct.Over_Current_Protection,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;	

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//短路保护转换为字符串
	my_itoa(DataStruct.Short_Current,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//快充支持转换为字符串
	my_itoa(DataStruct.Quick_Charge,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//上电时间转换为字符串
	my_itoa(DataStruct.Poweron_Time,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//效率转换为字符串
	my_itoa(DataStruct.Efficiency,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入逗号分隔符
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct)));
	if(res!=0) return 1;
	res=f_write(&Fsrc,&CommaStr,1,&check_count);
	if(res!=0||check_count!=1)	return 1;

	//测试结果转换为字符串
	my_itoa(DataStruct.Test_Subsequence,TempStr);
	// 写入数据
	res=f_lseek(&Fsrc,378+(count*sizeof(DataStruct))+9);
	if(res!=0) return 1;
	res=f_write(&Fsrc,TempStr,2,&check_count);
	if(res!=0||check_count!=2)	return 1;

	// 写入换行符
	res=f_lseek(&Fsrc,376);
	if(res!=0) return 1;
	res=f_write(&Fsrc,&EnterStr,2,&check_count);
	if(check_count!=2||res!=0) return 1;

	// 冲洗一下数据
	
	//重新写入统计数据
	count+=1;
	// 将初始总数转换为字符串
	my_itoa(count,countStr);
	// 写入初始总数
	res=f_write(&Fsrc,&countStr,5,&check_count);
	if(check_count!=5||res!=0) return 1;
	//关闭文件
	res=f_close(&Fsrc);
	if(res!=0)	return 22;
	//关闭目录
	res=f_closedir(&Dir);
	if(res!=0) return 23;
	return 0;
}

/**
 * 创建新的数据文件并初始化文件头部
 * @param  File_Name [文件名]
 * @return           [结果代码]
 */
u8 Creat_FileHeader(char *File_Name)
{
	u8 res=0;
	FIL 	Fsrc;	//定义文件对象
	UINT check_count=0;
	u16 count=0;
	char countStr[5];

	//创建新的CSV文件并初始化文件头部		
	res=f_open(&Fsrc,File_Name,FA_READ|FA_WRITE|FA_CREATE_NEW);
	// 创建出错，结束
	if(res!=0) return 1;
	//空间预分配   偏移5000
	res=f_lseek(&Fsrc,5000);
	if(res!=0) return 1;
	// 返回文件开始位置
	res=f_lseek(&Fsrc,0);
	if(res!=0) return 1;
	//写入文件头部标签
	res=f_write(&Fsrc,Data_FileHeader,370,&check_count);
	if(check_count!=370||res!=0) return 1;
	//初始化总数
	res=f_lseek(&Fsrc,371);
	if(res!=0) return 1;
	// 将初始总数转换为字符串
	my_itoa(count,countStr);
	// 写入初始总数
	res=f_write(&Fsrc,&countStr,5,&check_count);
	if(check_count!=5||res!=0) return 1;
	// 写入换行符
	res=f_lseek(&Fsrc,376);
	if(res!=0) return 1;
	res=f_write(&Fsrc,&EnterStr,2,&check_count);
	if(check_count!=2||res!=0) return 1;
	//关闭文件
	res=f_close(&Fsrc);
	if(res!=0)  return 1;
}

u8 Get_DataNum(u8 *path)
{
	u8 res;
	u16 rval;
	DIR tdir;
	FILINFO tfileinfo;
	u8 *fn;
	res=f_opendir(&tdir,(const TCHAR *)path);
	if(res!=0) return 1;
	while(1)
	{
		res=f_readdir(&tdir,&tfileinfo);
	}
}

int my_atoi(char s[])
{
	int i,n,sign;
	for(i=0;isspace(s[i]);i++)//跳过空白符;
	sign=(s[i]=='-')?-1:1;
	if(s[i]=='+'||s[i]==' -')//跳过符号
		i++;
	for(n=0;isdigit(s[i]);i++)
				 n=10*n+(s[i]-'0');//将数字字符转换成整形数字
	return sign *n;
}

void my_itoa(int n,char s[])
{
	int i,j,sign;
	if((sign=n)<0)//记录符号
	n=-n;//使n成为正数
	i=0;
	do{
				 s[i++]=n%10+'0';//取下一个数字
	}
	while ((n/=10)>0);//删除该数字
	if(sign<0)
	s[i++]='-';
	s[i]='\0';
	for(j=i;j>=0;j--)//生成的数字是逆序的，所以要逆序输出
				 printf("%c",s[j]);
}



