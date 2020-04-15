
#include "s3c2440_soc.h"

void sdram_init(void)
{

	BWSCON = 0x22000000;     //初始化BWSCON,选用BANK6,7

	BANKCON6 = 0x00018001;
	BANKCON7 = 0x00018001;

	REFRESH = 0x8404f5;
	
	BANKSIZE = 0x000000b1;

	MRSRB6 = 0x00000020;     //设置CL为2clock，具体查手册
	MRSRB7 = 0x00000020;     //设置CL为2clock，具体查手册
		
}

int sdram_test(void)
{
		
	int i;
	volatile unsigned char *p = (volatile unsigned char *)0x30000000; //p指针指向sdram的地址

	//写数据到sdram，从0x30000000到0x30001000都写入0x22
	for(i=0;i<=1000;i++)
	{
		p[i] = 0x22;
	}

	//从sdram读数据
	for(i=0;i<=1000;i++)
	{
		if(p[i] != 0x22)
			return -1;
	}
	return 0;
}


