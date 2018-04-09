#include "CSV_Database.h"
// 使用itoa和atoi函数加入
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "string.h"
#include "ff.h"
#include "sys.h"
#include "stm32f4xx.h"
#include "rtc.h"
#include "usart1.h"
#include "rtthread.h"
#include "app.h"



//定义数据文件头部标签 
const char Data_FileHeader[]="Serial_Number,Test_Time,Ripple_Voltage,Vout_Max,Cout_Max,"
		"Over_Voltage_Protection,Over_Current_Protection,Short_Current,Quick_Charge,Poweron_Time,Efficiency,Test_Subsequence," ;
const char CommaStr=',';
const char EnterStr[2]={0x0d,0x0a};

TestStandard_Type TestStandard_Arrary[4];
u8 Current_event;	//定义全局用到的当前标准结构体

/**
 * 测试数据信息存储到文件系统
 * @param  DataStruct [description]
 * @return            [description]
 *	返回代码
 */
u8 Test_WriteData(TestData_Type DataStruct,char * File_Title)
{
	u8 res;
	FIL 	Fsrc;									//定义文件对象
	DIR   	Dir;									//定义目录对象
	u16 	count=0;								//定义统计数
	u16		count_CRC=0;																		
	u32   	REsize;
	char 	File_Name[30];							//定义文件名数组
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
	
//	printf("year=%d",DataStruct.Test_Time[0]);
//	printf("month=%d\n",DataStruct.Test_Time[2]);
	

	//生成目录名
	sprintf(Dir_Name,"0:/%02d%02d%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	//生成文件名
	strcpy(File_Name,Dir_Name);
	strcat(File_Name,"/");
	strcat(File_Name,File_Title);
	strcat(File_Name,".csv");
	printf("%s\r\n",File_Name);
	//打开目录
	res=f_opendir(&Dir,(const TCHAR*)Dir_Name);
	if(res==FR_NO_PATH)
	{
		res=f_mkdir((const TCHAR*)Dir_Name);
		if(res!=0) return 1;		//创建目录失败返回
		res=f_opendir(&Dir,(const TCHAR*)Dir_Name);
		if(res!=0) return 2;		//打开目录失败返回
	}
	else if(res!=0) return 2;

	//打开文件
	res=f_open(&Fsrc,File_Name,FA_READ|FA_WRITE);
	// 如果文件并不存在的打开错误则创建文件
	if(res!=0)
	{ 
		if(Creat_FileHeader(File_Name)!=0)
		return 3;			//创建文件头失败返回
		res=f_open(&Fsrc,File_Name,FA_READ|FA_WRITE);
		if(res!=0) return 3;  //仍不能成功打开文件
	}
	//读取文件数据总量并转换为数值(偏移量：371 字节数：5)
	res=f_lseek(&Fsrc,371);
	if(res!=0) return 1;
	// 读出数据
	res=f_read(&Fsrc,&countStr,5,&check_count);
	if(res!=0||check_count!=5) return 1; 
	// 转换格式
	printf("count=%s",countStr);
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
	//空间预分配   偏移3000
	res=f_lseek(&Fsrc,3000);
	if(res!=0) return 1;
	// 返回文件开始位置
	res=f_lseek(&Fsrc,0);
	if(res!=0) return 1;
	//写入文件头部标签   文件标签长度： 
	res=f_write(&Fsrc,Data_FileHeader,sizeof(Data_FileHeader),&check_count);
	if(check_count!=sizeof(Data_FileHeader)||res!=0) return 1;
	//初始化总数
	res=f_lseek(&Fsrc,sizeof(Data_FileHeader));
	if(res!=0) return 1;
	// 将初始总数转换为字符串
	my_itoa(count,countStr);
	// 写入初始总数
	res=f_write(&Fsrc,&countStr,sizeof(countStr),&check_count);
	if(check_count!=sizeof(countStr)||res!=0) return 1;
	// 写入换行符
	res=f_lseek(&Fsrc,sizeof(Data_FileHeader)+sizeof(countStr));	//移动到指针位置
	if(res!=0) return 1;
	res=f_write(&Fsrc,&EnterStr,2,&check_count);
	if(check_count!=2||res!=0) return 1;
	
	//关闭文件
	res=f_close(&Fsrc);
	if(res!=0)  return 1;
}


/*
 * 创建新的批量目录
 */
u8 Creat_NewBatchDir(char* path)
{
	u8 res=0;
	
	res=f_mkdir(path);
	
	return res;
}



/*
 * 参数指标存储函数
 */
u8 Modify_TestParameters(TestParameters_Type* TestParameters_Structure,u8 Standard_code)
{
	u8 res;
	UINT check_count;
	FIL* Fsrc;
	
	Fsrc=rt_malloc(sizeof(FIL));
	if(Fsrc==NULL) return 1;
	
	res=f_open(Fsrc,"0:/setting.data",FA_READ|FA_WRITE);
	if(res!=0)
	{
		res=f_open(Fsrc,"0:/setting.data",FA_READ|FA_WRITE|FA_CREATE_NEW);
		/* 仍不能成功打开文件 */
		return 1; //返回错误码
		//空间预分配   偏移3000
		res=f_lseek(Fsrc,3000);
		if(res!=0) return 2;
	}
	//偏移(Standard_code*sizeof(TestParameters_Type)
	res=f_lseek(Fsrc,Standard_code*sizeof(TestParameters_Type));
	if(res!=0) return 2;
	res=f_write(Fsrc,TestParameters_Structure,sizeof(TestParameters_Type),&check_count);
	if(check_count!=sizeof(TestParameters_Type)||res!=0) return 1;	//写入出错,退出
	
	rt_free(Fsrc);
	
	return 0;
}

/*
 * 读取测试指标信息
 */
u8 Read_TestParameters(TestParameters_Type* TestParameters_Structure,u8 Standard_code)
{
	u8 res;
	UINT check_count;
	FIL* Fsrc;
	
	Fsrc=rt_malloc(sizeof(FIL));
	if(Fsrc==NULL) return 1;

	
	res=f_open(Fsrc,"0:/setting.data",FA_READ);
	if(res != 0) return 1;
	//偏移(Standard_code*sizeof(TestParameters_Type)
	res=f_lseek(Fsrc,Standard_code*sizeof(TestParameters_Type));
	if(res!=0) return 2;
	res=f_read(Fsrc,&TestParameters_Structure,sizeof(TestParameters_Type),&check_count);
	if(res!=0||check_count!=sizeof(TestParameters_Type)) return 3;

	rt_free(Fsrc);
	
	return 0;	
}

/*
 * 获取批量目录信息
 * 使用全局字符串数组Global_str[10][10]存储目录信息
 * 返回 获取到条目的数量
 */
u8 Scan_BatchDir(u8 start_val,u8 end_val)
{
	FRESULT res;	//FATFS执行返回值
	u8 i=0;
	u8 count=0;
	DIR tdir;
	FILINFO fileinfo;
	
	/* 防止参数错误 */
	if(end_val<start_val) return 0;
	
	if(end_val-start_val>6) return 0;
	
	res = f_opendir(&tdir,"0:");  //打开一个目录
	if(res != FR_OK) return 0;
	
	while(1)
	{
		res=f_readdir(&tdir,&fileinfo);	//读取目录下的一个文件
		if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
		if(fileinfo.fname[0]>='0'&&fileinfo.fname[0]<='9')
		{
			/* 文件名在提取窗口内，提取文件名 */
			if(i>=start_val&&i<=end_val)
			{
				strcpy(Global_str[count++],fileinfo.fname);
			}
			i++;
		}
	}
	if(count<5)
	for(i=count;i<=4;i++)
		strcpy(Global_str[i]," ");
	
	return count;
}




//支持含小数点的数值字符,但一律转换为无符号整型数值
unsigned int my_atoi(char s[])
{
	u8 i;
	u16 n=0;
	i=0;
	while(s[i]!='\0')
	{
		if(s[i]>='0'&&s[i]<='9') n=10*n+(s[i]-'0');
		i++;
	}
	return n;
}

void my_itoa(int n,char str[])
{
	int i,j,sign,k;
	char s[10];
	if((sign=n)<0)//记录符号
	n=-n;//使n成为正数
	i=0;
	k=0;
	do{
				 s[i++]=n%10+'0';//取下一个数字
	}
	while ((n/=10)>0);//删除该数字
	if(sign<0)
	s[i++]='-';
	s[i]='\0';
	for(j=i-1;j>=0;j--)//生成的数字是逆序的，所以要逆序输出
	{
		str[k]=s[j];
		k++;
	}
	str[k]='\0';
}

//输出加入小数点的字符串
void my_itoa_Dot(int n,char str[],int dot_num)
{
	int i,j,sign,k;
	char s[10],s2[10];
	if((sign=n)<0)//记录符号
	n=-n;//使n成为正数
	i=0;
	k=0;
	do{
				 s[i++]=n%10+'0';//取下一个数字
	}
	while ((n/=10)>0);//删除该数字
	if(sign<0)
	s[i++]='-';
	s[i]='\0';
	for(j=i-1;j>=0;j--)//生成的数字是逆序的，所以要逆序输出
	{
		s2[k]=s[j];
		k++;
	}
	s2[k]='\0';
	i=0;
	if(k-dot_num<=0)
	{
		str[i++]='0';
		str[i++]='.';
		if(k-2<0) str[i++]='0';
		else str[i++]=s2[k-2];
		if(k-1<0) str[i++]='0';
		else str[i++]=s2[k-1];
		str[i]=s[k];
	}
	else
	{
		for(i=0;i<k-dot_num;i++)
			str[i]=s2[i];
	str[i]='.';
	j=dot_num;
	do{
	i++;
	str[i]=s2[k-j];
	j--;
	}while(j>=0);
	}
}



