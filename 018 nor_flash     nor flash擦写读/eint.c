#include "s3c2440_soc.h"

typedef void(*irq_fp)(int);  //声明irq_fp为指向函数的指针类型，该函数没有返回整型值,有(int)参数//
irq_fp irq_handle[32];

/* EINTPEND，可通过读此寄存器知道EINT4-EINT23哪个发生中断，请中断时可往里面写1 */

/* SRCPND 用来显示哪个中断产生了, 需要清除对应位
 * EINT0-BIT[0]
 * EINT2-BIT[2]
 * EINR8_23-BIT[5]
 */

/* INTMOD 用来设置发生中断模式（IRQ和FIQ）
 * 0-IRQ，1-FIQ，默认为0
 */

/* INTMSK 用来屏蔽中断, 1-masked
 * EINT0-BIT[0]
 * EINT2-BIT[2]
 * EINR8_23-BIT[5]
 */
 /* 初始化中断控制器 */


//	void interrupt_init(void)
//	{
//	INTMSK &=~ ((1<<0)|(1<<2)|(1<<5)); 	
//	INTMSK &= ~(1<<10);  /* enable timer0 int */
//	}

 

/* INTPND 用来显示当前优先级最高的、正在发生的中断, 需要清除对应位
 * EINT0-BIT[0]
 * EINT2-BIT[2]
 * EINR8_23-BIT[5]
 */
 
/* INTOFFSET 用来显示INTPND中哪一位被设置为1
 * 这个位可以通过清除SRCPND和INTPND来自动清除。
 */




/* 创立一个注册函数，用来储存中断号和中断调用函数 */
void register_irq(int irq, irq_fp fp)
{
	irq_handle[irq] = fp;
	INTMSK &=~ (1<<irq);	
}

/* 中断处理函数
 * 先分辨中断源，对不同的中断源进行处理
 * 后清中断
 */

void key_irq_handle(int flag)
{
	unsigned int val = EINTPEND;
	unsigned int valf = GPFDAT;
	unsigned int valg = GPGDAT;

	if (flag == 0)
	{
		if (valf & (1<<0)) /* s2 --> gpf6 */
		{
			/* 松开 */
			GPFDAT |= (1<<6);
		}
		else
		{
			/* 按下 */
			GPFDAT &= ~(1<<6);
		}

	}
	else if (flag == 2)
	{
		if (valf & (1<<2)) /* s3 --> gpf5 */
		{
			/* 松开 */
			GPFDAT |= (1<<5);
		}
		else
		{
			/* 按下 */
			GPFDAT &= ~(1<<5);
		}
	}
	else if (flag == 5)
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (valg & (1<<3)) /* s4 --> gpf4 */
			{
				/* 松开 */
				GPFDAT |= (1<<4);
			}
			else
			{
				/* 按下 */
				GPFDAT &= ~(1<<4);
			}
		}
		else if (val & (1<<19)) /* eint19 */
		{
			if (valg & (1<<11))
			{
				/* 松开 */
				/* 熄灭所有LED */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* 按下: 点亮所有LED */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}	
	EINTPEND = val;
}


/* 
 *初始化按键并将其设置为中断源
 *S2:EINT0:GPF0 S3:EINT2:GPF2 S4:EINT11:GPG3 S5:EINT19:GPG11
 *设置中断方式：双边触发
 */
void key_eint_int(void)
{
	/* 
	*配置按键
	*设置寄存器，先请零、后设置
	*/
	GPFCON &=~((3<<0)|(3<<4));
	GPFCON |= ((2<<0)|(2<<4)); //设置S2、S3

	GPGCON &=~((3<<6)|(3<<22));
	GPGCON |= ((2<<22)|(2<<6)); //设置S4、S5

	EXTINT0 |= ((7<<0)|(7<<8)); //设置 EINT0、EINT2

	EXTINT1 |= (7<<12); //设置EINT11

	EXTINT2 |= (7<<12); //设置EINT19

	EINTMASK &=~ ((1<<11)|(1<<19)); //使能EINT11,19

	/*
	*设置LED灯
	*配置GPF4，5，6为输出口
	*先对寄存器CPFCON清0
	*后再配置寄存器，是GPF4,5,6为输出口
	*/
	
	GPFCON &=~ ((3<<8)|(3<<10)|(3<<12));
	GPFCON |=  ((1<<8)|(1<<10)|(1<<12));

	register_irq(0, key_irq_handle);
	register_irq(2, key_irq_handle);
	register_irq(5, key_irq_handle);
}

void handle_irq_c(void)
{
	int bit = INTOFFSET;

	irq_handle[bit](bit);
	SRCPND = (1<<bit);
	INTPND = (1<<bit);
}


