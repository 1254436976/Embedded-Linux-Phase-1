
#include "s3c2440_soc.h"

void uart0_init()
{
	/*
	*设置GPH寄存器，使GPH2,GPH3端口工作在TXD0,RXD0
	*使能内部上拉
	*/
	GPHCON &=~ ((3<<4)|(3<<6));
	GPHCON |=  ((2<<4)|(2<<6));

	GPHUP &=~ ((1<<2)|(1<<3)); 
	
	/*
	* 设置UART线寄存器
	* 波特率为115200 8n1：8位数据位，1个停止位，n个奇偶检验位
	*/
	ULCON0 = 0x00000003; 

	/* 
	* 设置UART控制寄存器 
	* 时钟频率选择为PCLK、中断请求模式
	*/
	UCON0 = 0x00000005;

	/*
	* 设置UARDIVn    = (int)( UART clock / ( buad rate x 16) ) –1
	* UART clock = PCLK = 50MHZ,  buad rate = 115200
	* UARDIVn=26
	*/
	UBRDIV0 = 26;
	
}

int putchar(int c)
{
	
	while(!(UTRSTAT0 & (1<<2)));
	UTXH0 = (unsigned char )c;
	

}

int getchar(void)
{
	while(!(UTRSTAT0 & (1<<0)));
	return URXH0;
}

int puts(const char *s)
{
	while(*s)
	{
		putchar(*s);
		s++;
	}
}


