#include "my_printf.h"
#include "string_utils.h"


#define  NOR_FLASH_BASE 0  //nor--cs0，base addr :0



/* 构建一个函数，功能:往地址里面写东西 */
void nor_write_addr(unsigned int base,unsigned int offset,unsigned int val )
{
	volatile unsigned short *p = (volatile unsigned short *)(base + (offset<<1));
	*p = val;
}
/* 封装nor_write_addr */
void nor_cmd(unsigned int offset,unsigned int cmd)
{
	nor_write_addr(NOR_FLASH_BASE,offset,cmd);
}

/* 构建一个函数，功能：读取地址中的信息 */
unsigned int nor_read_addr(unsigned int base,unsigned int offset)
{
	volatile unsigned short *p = (volatile unsigned short *)(base + (offset<<1));
	return *p;
}

/* 封装nor_read_addr */
unsigned int nor_data(unsigned int offset)
{
	return nor_read_addr(NOR_FLASH_BASE,offset);
}


/* 识别nor flash */
void do_scan_nor_flash(void)
{
	/* 各变量对应含义
	 * 用到的标志位
	 * 设备容量大小
	 * 厂家ID，设备ID
	 * region的数量、第一个region的信息地址
	 * blocks的数量、block的地址、block的大小
	 */
	char str[4];  
	int i,j,flag;
	int device_size;
	int vendor,device;
	int regions,regions_info_base;
	int blocks,block_addr,block_size;

	/* 打印设备ID 
	 * 1、先解锁
	 * 2、后根据信息输入对应的nor_cmd指令
	 * 3、读取数据后要reset
	 */
	nor_cmd(0x555,0xaa);
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0x90);
	
	vendor = nor_data(0);
	device = nor_data(1);

	nor_cmd(0,0xf0);
	
	/* 
	 * 1、首先进入cfi模式
	 * 2、对获取信息的不同，执行nor_cmd进行写操作
	 * 3、执行nor_data，读出对应地址的信息
	 */
	nor_cmd(0x55,0x98);
	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';
	printf("\n\r");
	printf("str = %s\n\r",str);
		
	/* 打印容量 */
	device_size = 1<<(nor_data(0x27));    //2的nor_data(0x27)次方
	printf("Vendor ID = 0x%x, Device ID = 0x%x, Nor Flash Size = 0x%x, %dM\n\r", vendor, device, device_size, (device_size/(1024*1024)));

	/* 打印各个扇区的起始地址 */
	/* 名词解释:
	 *    erase block region : 里面含有1个或多个block, 它们的大小一样
	 * 一个nor flash含有1个或多个region
	 * 一个region含有1个或多个block(扇区)

	 * Erase block region information:
	 *    前2字节+1    : 表示该region有多少个block 
	 *    后2字节*256  : 表示block的大小
	 */
	regions = nor_data(0x2C);
	regions_info_base = 0x2d;
	block_addr = 0;
	flag = 0;
	printf("\n\r");
	printf("Block/Sector Start Address:\n\r");

	for(i = 0;i<regions;i++)
	{
		
		blocks = nor_data(regions_info_base)+(nor_data(regions_info_base+1)<<8)+1;     //计算该region中blocks的个数
		block_size = 256*(nor_data(regions_info_base+2)+(nor_data(regions_info_base+3)<<8));	  //计算每个block的大小
		regions_info_base +=4;   //使得进入下次循环时地址为下一个region的信息地址
		
		for(j=0;j<blocks;j++)
		{
		
		   /* 打印每个block的起始地址 */
			printHex(block_addr);
			putchar(' ');
			flag++;
			block_addr +=block_size;
			if (flag % 5 == 0)
				printf("\n\r");
			
		}
	}
	printf("\n\r");
	/* 退出CFI模式 */
	nor_cmd(0, 0xf0);
}

/* 擦除nor flash某个扇区 
 * 1、获取扇区地址
 * 2、按照手册执行擦除命令
 * 3、判断Q6位是否擦除完毕
 */
void do_erase_nor_flash(void)
{


}

/* 编写某个地址 
 * 1、获取地址
 * 2、按照手册执行写命令
 * 3、判断Q6位是否写完毕
 */
void do_write_nor_flash(void)
{


}

/* 读某个地址 
 * 1、获取地址
 * 2、打印地址信息
 */
void do_read_nor_flash(void)
{
	int i,j;
	unsigned int addr;
	unsigned char c,str[16];
	volatile unsigned char *p; 
	
	/* 获取地址 */
	printf("Enter the address to read: ");
	addr = get_uint();
	p = (volatile unsigned char *)addr;

	printf("Data: \n\r");
	/* 打印地址信息
	 * 1、固定长度64字节
	 * 2、按照市面上的标准以16个字节为一行，前为数值，后为字符
	 * 3、分辨字符是否可视
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
		/* 打印菜单，供选择测试内容 */
		printf("[S] Scan nor flash\n\r");
		printf("[E] Erase nor flash\n\r");
		printf("[W] Write nor flash\n\r");
		printf("[R] Read nor flash\n\r");
		printf("[Q] Quit nor flash\n\r");
		printf("Enter Selection: ");

		c = getchar();
		printf("%c\n\r",c);
		
		/* 测试内容：
		 * 1、识别nor flash
		 * 2、擦除nor flash某个扇区
		 * 3、编写某个地址
		 * 4、读某个地址
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
