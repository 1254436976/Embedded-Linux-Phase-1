#include "s3c2440_soc.h"
#include "my_printf.h"

void nand_init(void)
{
#define  TACLS   0
#define  TWRPH0  1
#define  TWRPH1  0
	/*����NAND FLASH��ʱ��*/
	NFCONF = (TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4);
	/*ʹ��NAND FLASH������,��ʼ��ECC����ֹƬѡ*/
	NFCONT = (1<<4) | (1<<1) | (1<<0);
}

void nand_select(void)
{
	/*ʹ��Ƭѡ*/
	NFCONT &=~(1<<1);
}

void nand_deselect(void)
{
	/*��ֹƬѡ*/
	NFCONT |= (1<<1);
}

void nand_cmd(unsigned char cmd)
{
	volatile int i;
	NFCCMD = cmd;
	for(i=0; i<10; i++);
}

void nand_addr_byte(unsigned char addr)
{
	volatile int i;
	NFADDR = addr;
	for(i=0; i<10; i++);
}

unsigned char nand_data(void)
{
	return	NFDATA;
}

void nand_w_data(unsigned char val)
{
	NFDATA = val;
}


void wait_ready(void)
{
	while (!(NFSTAT & 1));
}

void nand_chip_id(void)
{ 
	unsigned char buf[5]={0};
	
	nand_select(); 
	nand_cmd(0x90);
	nand_addr_byte(0x00);

	buf[0] = nand_data();
	buf[1] = nand_data();	
	buf[2] = nand_data();
	buf[3] = nand_data();
	buf[4] = nand_data();	
	nand_deselect(); 	

	printf("maker   id  = 0x%x\n\r",buf[0]);
	printf("device  id  = 0x%x\n\r",buf[1]);	
	printf("3rd byte    = 0x%x\n\r",buf[2]);		
	printf("4th byte    = 0x%x\n\r",buf[3]);			
	printf("page  size  = %d kb\n\r",1  <<  (buf[3] & 0x03));	
	printf("block size  = %d kb\n\r",64 << ((buf[3] >> 4) & 0x03));	
	printf("5th byte    = 0x%x\n\r",buf[4]);

	
}


void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int i = 0;
	int page = addr / 2048;
	int col  = addr & (2048 - 1);
	
	nand_select(); 

	while (i < len)
	{
		/* ����00h���� */
		nand_cmd(00);

		/* ������ַ */
		/* col addr */
		nand_addr_byte(col & 0xff);
		nand_addr_byte((col>>8) & 0xff);

		/* row/page addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff);

		/* ����30h���� */
		nand_cmd(0x30);

		/* �ȴ����� */
		wait_ready();

		/* ������ */
		for (; (col < 2048) && (i < len); col++)
		{
			buf[i++] = nand_data();			
		}
		if (i == len)
			break;

		col = 0;
		page++;
	}
	
	nand_deselect(); 	
}


int nand_erase(unsigned int addr, unsigned int len)
{
	
}

void nand_write(unsigned int addr, unsigned char *buf, unsigned int len)
{
	
}

void do_read_nand_flash(void)
{
	unsigned int addr;
	volatile unsigned char *p;
	int i, j;
	unsigned char c;
	unsigned char str[16];
	unsigned char buf[64];
	
	/* ��õ�ַ */
	printf("Enter the address to read: ");
	addr = get_uint();

	nand_read(addr, buf, 64);
	p = (volatile unsigned char *)buf;

	printf("Data : \n\r");
	/* ���ȹ̶�Ϊ64 */
	for (i = 0; i < 4; i++)
	{
		/* ÿ�д�ӡ16������ */
		for (j = 0; j < 16; j++)
		{
			/* �ȴ�ӡ��ֵ */
			c = *p++;
			str[j] = c;
			printf("%02x ", c);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* ���ӡ�ַ� */
			if (str[j] < 0x20 || str[j] > 0x7e)  /* �������ַ� */
				putchar('.');
			else
				putchar(str[j]);
		}
		printf("\n\r");
	}
}

void do_erase_nand_flash(void)
{
	
}


void do_write_nand_flash(void)
{
	
}


void nand_flash_test(void)
{
	char c;

	while (1)
	{
		/* ��ӡ�˵�, ������ѡ��������� */
		printf("[s] Scan nand flash\n\r");
		printf("[e] Erase nand flash\n\r");
		printf("[w] Write nand flash\n\r");
		printf("[r] Read nand flash\n\r");
		printf("[q] quit\n\r");
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
				do_erase_nand_flash();
				break;

			case 'w':
			case 'W':
				do_write_nand_flash();
				break;

			case 'r':
			case 'R':
				do_read_nand_flash();
				break;
			default:
				break;
		}
	}
}

 
