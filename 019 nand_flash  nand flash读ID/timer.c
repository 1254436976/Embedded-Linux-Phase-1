#include "s3c2440_soc.h"


/* 中断相关函数 */
void timer_irq(void)
{
	/* 点灯计数 */
	static int cnt = 0;
	int tmp;

	cnt++;

	tmp = ~cnt;
	tmp &= 7;
	GPFDAT &= ~(7<<4);
	GPFDAT |= (tmp<<4);
}

void timer0_init(void)
{

	/* 
	 * 初始化时钟：Timer 0
	 * =PCLK / {prescaler value+1} / {divider value}
	 * 50000000/(99+1)/16=31250
	 */
	TCFG0 = 0x63;    //设置prescaler value
	TCFG1 &=~ 0xf;   
	TCFG1 |= 0x0003;    //设置divider value

	/* 设置初值*/
	TCNTB0 = 15625;  //0.5s中断一次
	
	/* 加载初值，启动时钟 */
	TCON = (1<<1);   //手动更新，Update TCNTB0 & TCMPB0

	/* 设置为自动加载模式 */
	TCON &=~(1<<1);
	TCON |= (1<<0)|(1<<3);

	register_irq(10, timer_irq);	
}












