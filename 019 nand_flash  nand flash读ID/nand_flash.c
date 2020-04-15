#include "s3c2440_soc.h"
#include "my_printf.h"

void nand_init(void)
{
	#define  TACLS   0
	#define  TWRPH0  1
	#define  TWRPH1  0
	
	/* ��ʼ��ʱ�� TACLS = 1��TWRPH0 = 1��TWRPH1 = 0 */
	NFCONF = (0<<12)|(1<<8)|(0<<4);	
	
	/*ʹ��NAND FLASH������,��ʼ��ECC����ֹƬѡ*/
	NFCONT = (1<<4) | (1<<1) | (1<<0);

}

void nand_select(void)
{
	/* Ƭѡ�ź� */
	NFCONT &=~ (1<<1);
}

void nand_disselect(void)
{
	/* ��ֹƬѡ�ź� */
	NFCONT |= (1<<1);
}

/* д���� */
void nand_cmd(unsigned char cmd)
{
	volatile int i;
	NFCCMD = cmd;
	for(i=0; i<10; i++);
}

/* д��ַ */
void nand_addr(unsigned char addr)
{
	volatile int i;
	NFADDR = addr;
	for(i=0; i<10; i++);
}

/* ������ */
unsigned char nand_data(void)
{
	return NFDATA;
}

void nand_chip_id(void)
{
	unsigned char buf[5];
	
	//	1��Ƭѡ�ź�
	nand_select();

	//	2��д��ID����
	nand_cmd(0x90);

	//	3��д��ַ
	nand_addr(0x00);

	//	4����5������
	buf[0]  = nand_data();	//����ID
	buf[1]  = nand_data();	//�豸ID
	buf[2]  = nand_data();	//3rd
	buf[3]  = nand_data();	//4th
	buf[4]  = nand_data();	//5th

	//	5����ӡ����
	/* ��ӡ5�ζ�ȡ������ */
	printf("Maker  Code :0x%x\n\r",buf[0]);
	printf("Device Code :0x%x\n\r",buf[1]);
	printf("3rd Cycle   :0x%x\n\r",buf[2]);
	printf("4rd Cycle   :0x%x\n\r",buf[3]);
	printf("5rd Cycle   :0x%x\n\r",buf[4]);
	
	/* ͨ����ȡ����ӡҳ��С�Ϳ��С */
	printf("Page Size 	:%d KB\n\r",1 << (buf[3]&0x03));
	printf("Block Size  :%d KB\n\r",64 << ((buf[3]>>4)&0x03));
}

void nand_flash_test(void)
{
	char c;

	while (1)
	{
		/* ��ӡ�˵�, ������ѡ��������� */
		printf("[s] Scan  nand flash\n\r");
		printf("[e] Erase nand flash\n\r");
		printf("[w] Write nand flash\n\r");
		printf("[r] Read  nand flash\n\r");
		printf("[q] Quit\n\r");
		printf("Enter selection: ");

		c = getchar();
		printf("%c\n\r", c);

		/* ��������:
		 * 1. ʶ��nand flash
		 * 2. ����nand flashĳ������
		 * 3. ��дĳ����ַ
		 * 4. ��ĳ����ַ
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				return;
				break;
				
			case 's':
			case 'S':
				nand_chip_id();
				break;

			case 'e':
			case 'E':
				
				break;

			case 'w':
			case 'W':
				
				break;

			case 'r':
			case 'R':
				
				break;
			default:
				break;
		}
	}
}

