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

void nand_wait_ready(void)
{
	while (!(NFSTAT & 1));
}

void nand_write_data(unsigned char val)
{
	NFDATA = val;
}

/* Nand Flash ��ID */
void do_scan_nand_flash(void)
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

	//	6����ֹƬѡ
	NFCONT |= (1<<1);
	
	/* ͨ����ȡ����ӡҳ��С�Ϳ��С */
	printf("Page Size   :%d KB\n\r",1 << (buf[3]&0x03));
	printf("Block Size  :%d KB\n\r",64 << ((buf[3]>>4)&0x03));
}

/* Nand Flash ����ַ */
void nand_read(unsigned int addr,unsigned char *buf ,int len)
{
	int i =0 ;
	int col = (addr & 0xfff);	//ȡ����
	int page = (addr >> 12);	//ȡҳ��
	
	//	1��Ƭѡ
	nand_select();
	while(i<len)
	{
		//	2����00����
		nand_cmd(0x00);

		//	3����������ڵĵ�ַ
		/* ������ */
		nand_addr(col & 0xff);
		nand_addr((col >> 8)    & 0x0f);
		
		/* ������ */
		nand_addr((page & 0xff));
		nand_addr((page >> 8) & 0xff);
		nand_addr(page>> 16);

		//	4����30����
		nand_cmd(0x30);

		//	�ȴ�ʱ��
		nand_wait_ready();
		
		//	5��������
		/* ��֤����������2047��i<len */
		for(; (col < 2048) && (i < len);col++)
		{
			buf[i++] = nand_data();
		}
		if (i == len)
			break;

		col = 0;
		page++;
		
	}
	
	//	6��ȡ��Ƭѡ
	nand_disselect();
}

/* ��ĳ����ַ 
 * 1����ȡ��ַ
 * 2����ӡ��ַ��Ϣ
 */
void do_read_nand_flash(void)
{
	int				i,j;	
	unsigned int	addr;	
	unsigned char	c;		
	unsigned char	buf[64];
	unsigned char	str[16];
	volatile unsigned char *tmpbuf;

	//	1����ȡ��ַ
	printf("Enter the address to read: ");
	addr = get_uint();

	//	2��ִ�ж�����
	nand_read(addr,buf,64);
	tmpbuf = (volatile unsigned char *)buf;

	// 3����ӡ��ַ��Ϣ
	/* �̶�����64�ֽ�
	 * ���������ϵı�׼��16���ֽ�Ϊһ�У�ǰΪ��ֵ����Ϊ�ַ�
	 * �ֱ��ַ��Ƿ����
	 */
	 printf("Data: \n\r");
	 for(i=0;i<4;i++)
	 {
	 	for(j=0;j<16;j++)
	 	{
	 		c = *tmpbuf++;
			str[j] = c;
			printf("%02x ",c);	
		}
		printf("   ;");
		
		for(j=0;j<16;j++)
	 	{
	 		if((str[j] < 0x20) || (str[j] > 0x7E))
				printf(".");
			else
				printf("%c",str[j]);
		}
		printf("\n\r");
	 }
}

void nand_erase(unsigned int addr)
{
	int page = (addr >> 12);		//ȡҳ��
	
	//	1��ѡ��оƬ
	nand_select();

	//	2����60����
	nand_cmd(0x60);

	//	3�����������ڵ��е�ַ
	/* ������ */
	nand_addr((page & 0xff));
	nand_addr((page >> 8) & 0xff);
	nand_addr(page>> 16);

	//	4��������DO����
	nand_cmd(0xd0);

	//	5���ȴ�����
	nand_wait_ready();

	//	6������70����
	nand_cmd(0x70);

	//	7���ж��Ƿ�����ɹ�
	if((nand_data() & 0x01) == 0 )
	{
		printf("Nand Flash Erase Successful!\n\r");
	}
	else
	{
		printf("Nand Flash Erase Fail!\n\r");
	}
	//	8 ��ȡ��ѡ��
	nand_disselect();
	
}

void do_erase_nand_flash(void)
{
	unsigned int addr;	//�洢����ĵ�ַ
	
	/* ��ȡ��ַ */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	nand_erase(addr);
	printf("\n\r");
}

void nand_write(unsigned int addr,unsigned char *buf,int len)
{
	int i = 0;
	int col = (addr & 0xfff);	//ȡ����
	int page = (addr >> 12);	//ȡҳ��

	//	1��ѡ��оƬ
	nand_select();
	
	while(1)
	{
		//	2������80����
		nand_cmd(0x80);
		
		//	3���������е�ַ�����꣩

		/* ������ */
		nand_addr(col & 0xff);
		nand_addr((col >> 8)	& 0x0f);
		
		/* ������ */
		nand_addr((page & 0xff));
		nand_addr((page >> 8) & 0xff);
		nand_addr(page>> 16);

		
		//	4������д�������
		for(;(col<2048) && (i<len);)
		{
			nand_write_data(buf[i++]);
		}	

		//	5������10����
		nand_cmd(0x10);

		//	6���ȴ�һ��ʱ��
		nand_wait_ready();
		
		if(i == len)
		{
			break;
		}
		/* ������һҳ��д */
		col = 0;
		page++;		
	}

	//	6������70����
	nand_cmd(0x70);

	//	7���鿴�Ƿ�д�ɹ�
	if((nand_data() & 0x01) == 0 )
	{
		printf("Nand Flash Write Successful!\n\r");
	}
	else
	{
		printf("Nand Flash Write Fail!\n\r");
	}
	//	8��ȡ��Ƭѡ
	nand_disselect();
	printf("\n\r");
}

void do_write_nand_flash(void)
{
	/* ��������Ӧ����
	 * �洢����ĵ�ַ
	 * �洢д�����Ϣ
	 */
	unsigned int addr;
	unsigned char str[100];

	/* ��ȡ��ַ */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	/* ��ȡд����Ϣ */
	printf("Enter the address to write: ");
	gets(str);

	nand_write(addr,str,strlen(str)+1);
	printf("writing ...\n\r");
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
				do_scan_nand_flash();
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
