
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

int IsBootNorFlash(void)
{
	volatile unsigned int *p = (volatile unsigned int *)0;
	unsigned int val = *p;

	*p = 0x123;
	if(*p == 0x123)
	{
		*p = val;
		return 0;
	}
	else
	{
		return 1;
	}

}

void nand_init(void);
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);

/*功能：复制整个text,rodata,data段到SDRAM中*/
void copy_to_sdram(void)
{
	extern int start,__bss_start;    //建立外部变量，方便获取lds文件中的量
	int len;
	
	volatile unsigned int *text =(volatile unsigned int *) &start;      //text指向程序开头地址
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;    //end指向bss段开头的地址
	volatile unsigned int *src = (volatile unsigned int *)0;       //src指向0地址.即FLASH的开头地址
	len = ((int)&__bss_start) - ((int)&start);

	if(IsBootNorFlash())
	{
		while(text < end)
		{
			*text++ = *src++; 		
		}
	}
	else
	{
		nand_init();
		nand_read(src,text,len);
	}
	
}


/*功能：清楚全部bss段*/
void clean_bss(void)
{

	extern int __bss_start,_end;     //建立外部变量，方便获取lds文件中的量
	volatile unsigned int *_start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *end = (volatile unsigned int *)&_end;

	while(_start <= end)
	{
		*(_start)++ =0; 
	}
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


