#include "s3c2440_soc.h"


int main(void)
{
	int buttonF_check ,buttonG_check;

  	/*配置GPF4，5，6为输出口
	*先对寄存器CPFCON清0
	*后再配置寄存器，是GPF4,5,6为输出口
	*/
	
	GPFCON &=~ ((3<<8)|(3<<10)|(3<<12));
	GPFCON |=  ((1<<8)|(1<<10)|(1<<12));

	/*配置GPF0，2，GPG3为输入口
	*先对寄存器CPFCON。CPGCON清0
	*后再配置寄存器，是GPF0,2,GPG3为输入口
	*/
	
	GPFCON &=~ ((3<<0)|(3<<4));
	GPGCON &=~ ((3<<6));


	/*按键按下检测
	**操作逻辑：检测按键对应IO口的电平情况，按下则灯亮，否则灯灭
	*/

	while(1)
	{
		buttonF_check=GPFDAT;
		buttonG_check=GPGDAT;
		if(buttonF_check & (1<<0))    //s2--->gpf4
		{
			GPFDAT |= (1<<4); 
		}	
		else
		{
			GPFDAT &=~ (1<<4);	
		}
		if(buttonF_check & (1<<2))    //s3--->gpf5
		{
			GPFDAT |= (1<<5); 
		}	
		else
		{
			GPFDAT &=~ (1<<5);	
		}
		if(buttonG_check & (1<<3))    //s4--->gpg3
		{
			GPFDAT |= (1<<6); 
		}	
		else
		{
			GPFDAT &=~ (1<<6);	
		}

	}
	return 0;
}
