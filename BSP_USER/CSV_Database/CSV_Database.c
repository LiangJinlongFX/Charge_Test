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
#include "exfuns.h"



//定义数据文件头部标签 
const char Data_FileHeader[]="Serial_Number,Testing_Time,Vout_Max,Vout_Min,Cout_Max,"
		"ShortCircuit_Current,NoLoad_Power,Conversion_Efficiency,Ripple_Voltage,OCP,Quick_Charge,Text_Code,";

u8 Current_event;	//定义全局用到的当前标准结构体

/**
 * 测试数据信息存储到文件系统
 * @param  DataStruct [description]
 * @return            [description]
 * 本函数不包含FATFS挂载函数,需提前初始化
 */
u8 Test_WriteData(TestData_Type DataStruct,char* BatchName)
{
	u8 res;
	u16 	count=0;								//定义统计数
	u16   Str_Len;													
	u16		lseek_Size;							//文件读写指针变量
	char 	File_Name[30];					//定义文件名数组
	char 	TempStr[30];
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

	//生成文件名
	sprintf(TempStr,"%02d%02d%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	//生成文件名
	strcpy(File_Name,"0:/");
	strcpy(File_Name,BatchName);	//加入批量名
	strcat(File_Name,"/");
	strcat(File_Name,TempStr);
	strcat(File_Name,".csv");

	//打开文件
	res=f_open(&Fsrc,File_Name,FA_READ|FA_WRITE);
	/* 如果文件并不存在的打开错误则创建文件 */
	if(res!=0)
	{ 
		if(Creat_FileHeader(File_Name)!=0)
		return 1;				//创建文件头失败返回
		res=f_open(&Fsrc,File_Name,FA_READ|FA_WRITE);
		if(res!=0) return 2;  //仍不能成功打开文件
	}
	
	/* 读取当前文件数据总量并转换为数值 */
	lseek_Size=strlen(Data_FileHeader);	//偏移
	if(f_lseek(&Fsrc,lseek_Size))	return 1;
	/* 读出当前文件数据总量 */
	res=f_read(&Fsrc,&TempStr,5,&check_count);
	if(res!=0||check_count!=5) return 1; 
	/* 转换格式 */
	strcat(TempStr,"\0");
	count=my_atoi(TempStr);
	rt_kprintf("count=%d\r\n",count);
	/* 根据总数给要写入的数据体写入序列号 */
	DataStruct.Serial_Number=count+1;		
	/* 预估文件空间,如果文件空间不足则扩展文件空间 */
	if(Fsrc.fsize<lseek_Size+7+(count+1)*sizeof(DataStruct))
	{
		res=f_lseek(&Fsrc,lseek_Size+7+(count+1)*sizeof(DataStruct));
		if(res!=0) return 1;
	}

	/* 重新写入统计数据 */
	count+=1;
	/* 将初始总数转换为字符串 */
	sprintf(TempStr,"%5d\r\n",count);
	/* 写入初始总数 */
	lseek_Size=strlen(Data_FileHeader);	//偏移
	rt_kprintf("count_add=%x\r\n",lseek_Size);
	if(f_lseek(&Fsrc,	lseek_Size-7)) return 1;
	res=f_write(&Fsrc,&TempStr,strlen(TempStr),&check_count);
	if(check_count!=strlen(TempStr)||res!=0) return 1;
		
	/* 读写指针移至文件尾部 */
	lseek_Size=f_size(&Fsrc);		
	rt_kprintf("len=%x\r\n",lseek_Size);
	
	/* 序列号转换为字符串 */
	sprintf(TempStr,"%5d,",DataStruct.Serial_Number);		
	/* 写入序列号数据 5位字符 */
	if(f_lseek(&Fsrc,lseek_Size)) return 1;
	Str_Len=strlen(TempStr);
	res=f_write(&Fsrc,TempStr,Str_Len,&check_count);
	if(res!=0||check_count!=Str_Len)	return 1;

	if(f_sync(&Fsrc)) return 1;
	
	//时间转换为转换为字符串
	sprintf(TempStr,"%2d-%2d-%2d %2d:%2d:%2d,",DataStruct.Test_Time[0],DataStruct.Test_Time[1],
	DataStruct.Test_Time[2],DataStruct.Test_Time[3],DataStruct.Test_Time[4],DataStruct.Test_Time[5]);
	// 写入数据
	Str_Len=strlen(TempStr);
	res=f_write(&Fsrc,TempStr,Str_Len,&check_count);
	if(res!=0||check_count!=Str_Len)	return 1;

	if(f_sync(&Fsrc)) return 1;
	
	//最大输出电压,最小输出电压,最大输出电流,短路电流,空载功率,转换效率,纹波电压转换为字符串
	sprintf(TempStr,"%5.3f,%5.3f,%4.3f,%4.3f,%4.3f,%3.1f,%4f,",DataStruct.Vout_Max,DataStruct.Vout_Min,
	DataStruct.Cout_Max,DataStruct.Cout_short,DataStruct.Noload_Power,DataStruct.Efficiency,DataStruct.Ripple_Voltage);
	/* 写入数据 */
	Str_Len=strlen(TempStr);
	res=f_write(&Fsrc,TempStr,Str_Len,&check_count);
	if(res!=0||check_count!=Str_Len)	return 1;
	
	if(f_sync(&Fsrc)) return 1;

	/* 过流保护,快充支持,测试结果代码加换行符转换为字符串 */
	sprintf(TempStr,"%1d,%1d,%1d\r\n",DataStruct.Over_Current_Protection,DataStruct.Quick_Charge,DataStruct.Test_Subsequence);
	/* 写入数据 */
	Str_Len=strlen(TempStr);
	res=f_write(&Fsrc,TempStr,Str_Len,&check_count);
	if(res!=0||check_count!=Str_Len)	return 1;
	
	//关闭文件
	if(f_close(&Fsrc)) return 1;
	
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
	u16 lseek_Size;
	UINT check_count=0;
	char countStr[10];

	/* 创建新的CSV文件并初始化文件头部 */		
	if(f_open(&Fsrc,File_Name,FA_READ|FA_WRITE|FA_CREATE_NEW)) return 1;
	if(f_lseek(&Fsrc,200)) return 1;	//空间预分配
	/* 返回文件开始位置 */
	if(f_lseek(&Fsrc,0)) return 1;
	/* 写入文件头部标签 */
	lseek_Size=strlen(Data_FileHeader);
	res=f_write(&Fsrc,Data_FileHeader,lseek_Size,&check_count);
	if(check_count!=lseek_Size||res!=0) return 1;
	// 将初始总数转换为字符串
	sprintf(countStr,"%5d\r\n",0);
	// 写入初始总数及换行符
	res=f_write(&Fsrc,&countStr,strlen(countStr),&check_count);
	if(check_count!=strlen(countStr)||res!=0) return 1;

	//关闭文件
	if(f_close(&Fsrc)) return 1;
	
	return 0;
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
	char str[100];
	
	res=f_open(&Fsrc,"0:/set.dat",FA_READ|FA_WRITE);
	if(res!=0)
	{
		res=f_open(&Fsrc,"0:/set.dat",FA_READ|FA_WRITE|FA_CREATE_NEW);
		/* 仍不能成功打开文件 */
		return res; //返回错误码
		//空间预分配   偏移3000
	}
	sprintf(str,"%4d,%4d,%4d,%4d,%4d,%4d,%4d\r\n",TestParameters_Structure->Vout,TestParameters_Structure->Vout_Tolerance,
	TestParameters_Structure->Cout_Max,TestParameters_Structure->V_Ripple,TestParameters_Structure->Efficiency,TestParameters_Structure->Safety_Code,
	TestParameters_Structure->Quick_Charge);
	rt_kprintf("%s\r\n",str);
	res=f_lseek(&Fsrc,0x10+Standard_code*strlen(str));
	if(res!=0) return 2;
	res=f_write(&Fsrc,str,strlen(str),&check_count);
	if(check_count!=strlen(str)||res!=0) return 1;	//写入出错,退出
	res=f_close(&Fsrc);
	
	return res;
}

/*
 * 读取测试指标信息
 */
u8 Read_TestParameters(TestParameters_Type* TestParameters_Structure,u8 Standard_code)
{
	u8 res;
	UINT check_count;
	char str[100];
	
	res=f_open(&Fsrc1,"0:/set.dat",FA_READ);
	if(res != 0) 
	{
			if(res==4) First_writeTestParameters();	//是因为找不到配置文件错误则创建新的文件
			else return 1;
	}
	res=f_lseek(&Fsrc1,0x10+Standard_code*36);
	if(res!=0) return res;
	res=f_read(&Fsrc1,str,36,&check_count);
	if(res!=0||check_count!=36) return 3;
	
	
	
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

u8 First_writeTestParameters(void)
{
	u8 res;
	TestParameters_Type TestParameters_temp;
	
	TestParameters_temp.Vout=5100;
	TestParameters_temp.Vout_Tolerance=10;
	TestParameters_temp.Cout_Max=3000;
	TestParameters_temp.V_Ripple=200;
	TestParameters_temp.Efficiency=89;
	TestParameters_temp.Quick_Charge=0;
	TestParameters_temp.Safety_Code=0;
	
	res=Modify_TestParameters(&TestParameters_temp,0);
	res=Modify_TestParameters(&TestParameters_temp,1);
	res=Modify_TestParameters(&TestParameters_temp,2);
	res=Modify_TestParameters(&TestParameters_temp,3);
	
	return res;
}


//数值字符,但一律转换为无符号整型数值
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


void my_strncpy(char* str1,char* str2,u8 start_addr,u8 n)
{
	u8 i;
	
	str1+=start_addr;
	
	for(i=0;i<n;i++)
	{
		*str2++=*str1;
	}
}



