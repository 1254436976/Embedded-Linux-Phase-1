
#include "s3c2440_soc.h"

void delay(volatile int time)
{
	while(time--);
}

int main(void)
{
	/*配置LED灯对应寄存器*/
    /*配置GPF4，5，6为输出口
	*先对寄存器CPFCON清0
	*后再配置寄存器，是GPF4,5,6为输出口
	*/
	
	GPFCON &=~ ((3<<8)|(3<<10)|(3<<12));
	GPFCON |=  ((1<<8)|(1<<10)|(1<<12));

	while(1)
	{
		GPFDAT |= (1<<4);
		delay(50000000);
		GPFDAT &=~ (1<<4);
		GPFDAT |= (1<<5);
		delay(50000000);
		GPFDAT &=~ (1<<5);
		GPFDAT |= (1<<6);
		delay(50000000);
		GPFDAT &=~ (1<<6);	
	}
	return 0;
}
