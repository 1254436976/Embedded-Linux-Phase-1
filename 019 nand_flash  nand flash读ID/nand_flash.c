#include "s3c2440_soc.h"
#include "my_printf.h"

void nand_init(void)
{
	#define  TACLS   0
	#define  TWRPH0  1
	#define  TWRPH1  0
	
	/* 初始化时序 TACLS = 1、TWRPH0 = 1、TWRPH1 = 0 */
	NFCONF = (0<<12)|(1<<8)|(0<<4);	
	
	/*使能NAND FLASH控制器,初始化ECC，禁止片选*/
	NFCONT = (1<<4) | (1<<1) | (1<<0);

}

void nand_select(void)
{
	/* 片选信号 */
	NFCONT &=~ (1<<1);
}

void nand_disselect(void)
{
	/* 禁止片选信号 */
	NFCONT |= (1<<1);
}

/* 写命令 */
void nand_cmd(unsigned char cmd)
{
	volatile int i;
	NFCCMD = cmd;
	for(i=0; i<10; i++);
}

/* 写地址 */
void nand_addr(unsigned char addr)
{
	volatile int i;
	NFADDR = addr;
	for(i=0; i<10; i++);
}

/* 读数据 */
unsigned char nand_data(void)
{
	return NFDATA;
}

void nand_chip_id(void)
{
	unsigned char buf[5];
	
	//	1、片选信号
	nand_select();

	//	2、写读ID命令
	nand_cmd(0x90);

	//	3、写地址
	nand_addr(0x00);

	//	4、读5次数据
	buf[0]  = nand_data();	//厂商ID
	buf[1]  = nand_data();	//设备ID
	buf[2]  = nand_data();	//3rd
	buf[3]  = nand_data();	//4th
	buf[4]  = nand_data();	//5th

	//	5、打印数据
	/* 打印5次读取的数据 */
	printf("Maker  Code :0x%x\n\r",buf[0]);
	printf("Device Code :0x%x\n\r",buf[1]);
	printf("3rd Cycle   :0x%x\n\r",buf[2]);
	printf("4rd Cycle   :0x%x\n\r",buf[3]);
	printf("5rd Cycle   :0x%x\n\r",buf[4]);
	
	/* 通过提取，打印页大小和块大小 */
	printf("Page Size 	:%d KB\n\r",1 << (buf[3]&0x03));
	printf("Block Size  :%d KB\n\r",64 << ((buf[3]>>4)&0x03));
}

void nand_flash_test(void)
{
	char c;

	while (1)
	{
		/* 打印菜单, 供我们选择测试内容 */
		printf("[s] Scan  nand flash\n\r");
		printf("[e] Erase nand flash\n\r");
		printf("[w] Write nand flash\n\r");
		printf("[r] Read  nand flash\n\r");
		printf("[q] Quit\n\r");
		printf("Enter selection: ");

		c = getchar();
		printf("%c\n\r", c);

		/* 测试内容:
		 * 1. 识别nand flash
		 * 2. 擦除nand flash某个扇区
		 * 3. 编写某个地址
		 * 4. 读某个地址
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

