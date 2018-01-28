#include <finsh.h>
#include "HMI.h"

int mycmd(void)
{
	rt_kprintf("hello world!\r\n");
	return 0;
}

MSH_CMD_EXPORT(mycmd,my command test);

int print(int argc,char** argv)
{
	rt_kprintf("print ");
	
	if(argc>1)	
	{
		rt_kprintf("%s\r\n",argv[1]);
		HMI_Print(argv[1]);
	}
	return 0;
}

MSH_CMD_EXPORT(print,my command test);
