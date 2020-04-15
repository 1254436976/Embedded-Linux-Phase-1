#include "my_printf.h"
#include "string_utils.h"


#define  NOR_FLASH_BASE 0  //nor--cs0��base addr :0



/* ����һ������������:����ַ����д���� */
void nor_write_addr(unsigned int base,unsigned int offset,unsigned int val )
{
	volatile unsigned short *p = (volatile unsigned short *)(base + (offset<<1));
	*p = val;
}
/* ��װnor_write_addr */
void nor_cmd(unsigned int offset,unsigned int cmd)
{
	nor_write_addr(NOR_FLASH_BASE,offset,cmd);
}

/* ����һ�����������ܣ���ȡ��ַ�е���Ϣ */
unsigned int nor_read_addr(unsigned int base,unsigned int offset)
{
	volatile unsigned short *p = (volatile unsigned short *)(base + (offset<<1));
	return *p;
}

/* ��װnor_read_addr */
unsigned int nor_data(unsigned int offset)
{
	return nor_read_addr(NOR_FLASH_BASE,offset);
}


/* ʶ��nor flash */
void do_scan_nor_flash(void)
{
	/* ��������Ӧ����
	 * �õ��ı�־λ
	 * �豸������С
	 * ����ID���豸ID
	 * region����������һ��region����Ϣ��ַ
	 * blocks��������block�ĵ�ַ��block�Ĵ�С
	 */
	char str[4];  
	int i,j,flag;
	int device_size;
	int vendor,device;
	int regions,regions_info_base;
	int blocks,block_addr,block_size;

	/* ��ӡ�豸ID 
	 * 1���Ƚ���
	 * 2���������Ϣ�����Ӧ��nor_cmdָ��
	 * 3����ȡ���ݺ�Ҫreset
	 */
	nor_cmd(0x555,0xaa);
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0x90);
	
	vendor = nor_data(0);
	device = nor_data(1);

	nor_cmd(0,0xf0);
	
	/* 
	 * 1�����Ƚ���cfiģʽ
	 * 2���Ի�ȡ��Ϣ�Ĳ�ͬ��ִ��nor_cmd����д����
	 * 3��ִ��nor_data��������Ӧ��ַ����Ϣ
	 */
	nor_cmd(0x55,0x98);
	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';
	printf("\n\r");
	printf("str = %s\n\r",str);
		
	/* ��ӡ���� */
	device_size = 1<<(nor_data(0x27));    //2��nor_data(0x27)�η�
	printf("Vendor ID = 0x%x, Device ID = 0x%x, Nor Flash Size = 0x%x, %dM\n\r", vendor, device, device_size, (device_size/(1024*1024)));

	/* ��ӡ������������ʼ��ַ */
	/* ���ʽ���:
	 *    erase block region : ���溬��1������block, ���ǵĴ�Сһ��
	 * һ��nor flash����1������region
	 * һ��region����1������block(����)

	 * Erase block region information:
	 *    ǰ2�ֽ�+1    : ��ʾ��region�ж��ٸ�block 
	 *    ��2�ֽ�*256  : ��ʾblock�Ĵ�С
	 */
	regions = nor_data(0x2C);
	regions_info_base = 0x2d;
	block_addr = 0;
	flag = 0;
	printf("\n\r");
	printf("Block/Sector Start Address:\n\r");

	for(i = 0;i<regions;i++)
	{
		
		blocks = nor_data(regions_info_base)+(nor_data(regions_info_base+1)<<8)+1;     //�����region��blocks�ĸ���
		block_size = 256*(nor_data(regions_info_base+2)+(nor_data(regions_info_base+3)<<8));	  //����ÿ��block�Ĵ�С
		regions_info_base +=4;   //ʹ�ý����´�ѭ��ʱ��ַΪ��һ��region����Ϣ��ַ
		
		for(j=0;j<blocks;j++)
		{
		
		   /* ��ӡÿ��block����ʼ��ַ */
			printHex(block_addr);
			putchar(' ');
			flag++;
			block_addr +=block_size;
			if (flag % 5 == 0)
				printf("\n\r");
			
		}
	}
	printf("\n\r");
	/* �˳�CFIģʽ */
	nor_cmd(0, 0xf0);
}

/* ����nor flashĳ������ 
 * 1����ȡ������ַ
 * 2�������ֲ�ִ�в�������
 * 3���ж�Q6λ�Ƿ�������
 */
void do_erase_nor_flash(void)
{


}

/* ��дĳ����ַ 
 * 1����ȡ��ַ
 * 2�������ֲ�ִ��д����
 * 3���ж�Q6λ�Ƿ�д���
 */
void do_write_nor_flash(void)
{


}

/* ��ĳ����ַ 
 * 1����ȡ��ַ
 * 2����ӡ��ַ��Ϣ
 */
void do_read_nor_flash(void)
{
	int i,j;
	unsigned int addr;
	unsigned char c,str[16];
	volatile unsigned char *p; 
	
	/* ��ȡ��ַ */
	printf("Enter the address to read: ");
	addr = get_uint();
	p = (volatile unsigned char *)addr;

	printf("Data: \n\r");
	/* ��ӡ��ַ��Ϣ
	 * 1���̶�����64�ֽ�
	 * 2�����������ϵı�׼��16���ֽ�Ϊһ�У�ǰΪ��ֵ����Ϊ�ַ�
	 * 3���ֱ��ַ��Ƿ����
	 */
	 for(i=0;i<4;i++)
	 {

	 	for(j=0;j<16;j++)
	 	{
	 		c = *p++;
			str[j] = c;
			printf("0x02 ",c);
		}
		
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






void nor_flash_test(void)
{
	char c;
	while(1)
	{
		/* ��ӡ�˵�����ѡ��������� */
		printf("[S] Scan nor flash\n\r");
		printf("[E] Erase nor flash\n\r");
		printf("[W] Write nor flash\n\r");
		printf("[R] Read nor flash\n\r");
		printf("[Q] Quit nor flash\n\r");
		printf("Enter Selection: ");

		c = getchar();
		printf("%c\n\r",c);
		
		/* �������ݣ�
		 * 1��ʶ��nor flash
		 * 2������nor flashĳ������
		 * 3����дĳ����ַ
		 * 4����ĳ����ַ
		 */

		switch (c)
		{
			case 'Q':
			case 'q':
				return;
				break;
			case 'S':
			case 's':
				do_scan_nor_flash();
				break;
			case 'W':
			case 'w':
				do_write_nor_flash();
				break;
			case 'R':
			case 'r':
				do_read_nor_flash();
				break;
			case 'E':
			case 'e':
				do_erase_nor_flash();
				break;
			default:
				break;




		}
	}
}
