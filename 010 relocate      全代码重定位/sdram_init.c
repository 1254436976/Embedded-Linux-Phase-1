
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


