
#include "s3c2440_soc.h"

void delay(volatile int time)
{
	while(time--);
}

int main(void)
{
	/*����LED�ƶ�Ӧ�Ĵ���*/
    /*����GPF4��5��6Ϊ�����
	*�ȶԼĴ���CPFCON��0
	*�������üĴ�������GPF4,5,6Ϊ�����
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
