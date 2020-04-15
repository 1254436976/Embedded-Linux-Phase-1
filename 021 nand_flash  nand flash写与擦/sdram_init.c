
#include "s3c2440_soc.h"

void sdram_init(void)
{

	BWSCON = 0x22000000;     //��ʼ��BWSCON,ѡ��BANK6,7

	BANKCON6 = 0x00018001;
	BANKCON7 = 0x00018001;

	REFRESH = 0x8404f5;
	
	BANKSIZE = 0x000000b1;

	MRSRB6 = 0x00000020;     //����CLΪ2clock��������ֲ�
	MRSRB7 = 0x00000020;     //����CLΪ2clock��������ֲ�
		
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

/*���ܣ���������text,rodata,data�ε�SDRAM��*/
void copy_to_sdram(void)
{
	extern int start,__bss_start;    //�����ⲿ�����������ȡlds�ļ��е���
	int len;
	
	volatile unsigned int *text =(volatile unsigned int *) &start;      //textָ�����ͷ��ַ
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;    //endָ��bss�ο�ͷ�ĵ�ַ
	volatile unsigned int *src = (volatile unsigned int *)0;       //srcָ��0��ַ.��FLASH�Ŀ�ͷ��ַ
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


/*���ܣ����ȫ��bss��*/
void clean_bss(void)
{

	extern int __bss_start,_end;     //�����ⲿ�����������ȡlds�ļ��е���
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
	volatile unsigned char *p = (volatile unsigned char *)0x30000000; //pָ��ָ��sdram�ĵ�ַ

	//д���ݵ�sdram����0x30000000��0x30001000��д��0x22
	for(i=0;i<=1000;i++)
	{
		p[i] = 0x22;
	}

	//��sdram������
	for(i=0;i<=1000;i++)
	{
		if(p[i] != 0x22)
			return -1;
	}
	return 0;
}


