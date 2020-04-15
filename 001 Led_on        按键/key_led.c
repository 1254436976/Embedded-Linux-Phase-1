#include "s3c2440_soc.h"


int main(void)
{
	int buttonF_check ,buttonG_check;

  	/*����GPF4��5��6Ϊ�����
	*�ȶԼĴ���CPFCON��0
	*�������üĴ�������GPF4,5,6Ϊ�����
	*/
	
	GPFCON &=~ ((3<<8)|(3<<10)|(3<<12));
	GPFCON |=  ((1<<8)|(1<<10)|(1<<12));

	/*����GPF0��2��GPG3Ϊ�����
	*�ȶԼĴ���CPFCON��CPGCON��0
	*�������üĴ�������GPF0,2,GPG3Ϊ�����
	*/
	
	GPFCON &=~ ((3<<0)|(3<<4));
	GPGCON &=~ ((3<<6));


	/*�������¼��
	**�����߼�����ⰴ����ӦIO�ڵĵ�ƽ�����������������������
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
