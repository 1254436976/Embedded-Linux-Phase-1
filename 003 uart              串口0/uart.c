
#include "s3c2440_soc.h"

void uart0_init()
{
	/*
	*����GPH�Ĵ�����ʹGPH2,GPH3�˿ڹ�����TXD0,RXD0
	*ʹ���ڲ�����
	*/
	GPHCON &=~ ((3<<4)|(3<<6));
	GPHCON |=  ((2<<4)|(2<<6));

	GPHUP &=~ ((1<<2)|(1<<3)); 
	
	/*
	* ����UART�߼Ĵ���
	* ������Ϊ115200 8n1��8λ����λ��1��ֹͣλ��n����ż����λ
	*/
	ULCON0 = 0x00000003; 

	/* 
	* ����UART���ƼĴ��� 
	* ʱ��Ƶ��ѡ��ΪPCLK���ж�����ģʽ
	*/
	UCON0 = 0x00000005;

	/*
	* ����UARDIVn    = (int)( UART clock / ( buad rate x 16) ) �C1
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


