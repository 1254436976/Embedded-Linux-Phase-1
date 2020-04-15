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

void nand_wait_ready(void)
{
	while (!(NFSTAT & 1));
}

void nand_write_data(unsigned char val)
{
	NFDATA = val;
}

/* Nand Flash 读ID */
void do_scan_nand_flash(void)
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

	//	6、禁止片选
	NFCONT |= (1<<1);
	
	/* 通过提取，打印页大小和块大小 */
	printf("Page Size   :%d KB\n\r",1 << (buf[3]&0x03));
	printf("Block Size  :%d KB\n\r",64 << ((buf[3]>>4)&0x03));
}

/* Nand Flash 读地址 */
void nand_read(unsigned int addr,unsigned char *buf ,int len)
{
	int i =0 ;
	int col = (addr & 0xfff);	//取列数
	int page = (addr >> 12);	//取页数
	
	//	1、片选
	nand_select();
	while(i<len)
	{
		//	2、发00命令
		nand_cmd(0x00);

		//	3、发五个周期的地址
		/* 横坐标 */
		nand_addr(col & 0xff);
		nand_addr((col >> 8)    & 0x0f);
		
		/* 纵坐标 */
		nand_addr((page & 0xff));
		nand_addr((page >> 8) & 0xff);
		nand_addr(page>> 16);

		//	4、发30命令
		nand_cmd(0x30);

		//	等待时间
		nand_wait_ready();
		
		//	5、读数据
		/* 保证列数不超过2047，i<len */
		for(; (col < 2048) && (i < len);col++)
		{
			buf[i++] = nand_data();
		}
		if (i == len)
			break;

		col = 0;
		page++;
		
	}
	
	//	6、取消片选
	nand_disselect();
}

/* 读某个地址 
 * 1、获取地址
 * 2、打印地址信息
 */
void do_read_nand_flash(void)
{
	int				i,j;	
	unsigned int	addr;	
	unsigned char	c;		
	unsigned char	buf[64];
	unsigned char	str[16];
	volatile unsigned char *tmpbuf;

	//	1、获取地址
	printf("Enter the address to read: ");
	addr = get_uint();

	//	2、执行读函数
	nand_read(addr,buf,64);
	tmpbuf = (volatile unsigned char *)buf;

	// 3、打印地址信息
	/* 固定长度64字节
	 * 按照市面上的标准以16个字节为一行，前为数值，后为字符
	 * 分辨字符是否可视
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
	int page = (addr >> 12);		//取页数
	
	//	1、选中芯片
	nand_select();

	//	2、发60命令
	nand_cmd(0x60);

	//	3、发三个周期的行地址
	/* 纵坐标 */
	nand_addr((page & 0xff));
	nand_addr((page >> 8) & 0xff);
	nand_addr(page>> 16);

	//	4、发擦除DO命令
	nand_cmd(0xd0);

	//	5、等待周期
	nand_wait_ready();

	//	6、发出70命令
	nand_cmd(0x70);

	//	7、判断是否擦除成功
	if((nand_data() & 0x01) == 0 )
	{
		printf("Nand Flash Erase Successful!\n\r");
	}
	else
	{
		printf("Nand Flash Erase Fail!\n\r");
	}
	//	8 、取消选中
	nand_disselect();
	
}

void do_erase_nand_flash(void)
{
	unsigned int addr;	//存储输入的地址
	
	/* 获取地址 */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	nand_erase(addr);
	printf("\n\r");
}

void nand_write(unsigned int addr,unsigned char *buf,int len)
{
	int i = 0;
	int col = (addr & 0xfff);	//取列数
	int page = (addr >> 12);	//取页数

	//	1、选中芯片
	nand_select();
	
	while(1)
	{
		//	2、发出80命令
		nand_cmd(0x80);
		
		//	3、发出行列地址（坐标）

		/* 横坐标 */
		nand_addr(col & 0xff);
		nand_addr((col >> 8)	& 0x0f);
		
		/* 纵坐标 */
		nand_addr((page & 0xff));
		nand_addr((page >> 8) & 0xff);
		nand_addr(page>> 16);

		
		//	4、发送写入的数据
		for(;(col<2048) && (i<len);)
		{
			nand_write_data(buf[i++]);
		}	

		//	5、发出10命令
		nand_cmd(0x10);

		//	6、等待一段时间
		nand_wait_ready();
		
		if(i == len)
		{
			break;
		}
		/* 进行下一页的写 */
		col = 0;
		page++;		
	}

	//	6、发出70命令
	nand_cmd(0x70);

	//	7、查看是否写成功
	if((nand_data() & 0x01) == 0 )
	{
		printf("Nand Flash Write Successful!\n\r");
	}
	else
	{
		printf("Nand Flash Write Fail!\n\r");
	}
	//	8、取消片选
	nand_disselect();
	printf("\n\r");
}

void do_write_nand_flash(void)
{
	/* 各变量对应含义
	 * 存储输入的地址
	 * 存储写入的信息
	 */
	unsigned int addr;
	unsigned char str[100];

	/* 获取地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	/* 获取写入信息 */
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
