#include "s3c2440_soc.h"


/* �ж���غ��� */
void timer_irq(void)
{
	/* ��Ƽ��� */
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
	 * ��ʼ��ʱ�ӣ�Timer 0
	 * =PCLK / {prescaler value+1} / {divider value}
	 * 50000000/(99+1)/16=31250
	 */
	TCFG0 = 0x63;    //����prescaler value
	TCFG1 &=~ 0xf;   
	TCFG1 |= 0x0003;    //����divider value

	/* ���ó�ֵ*/
	TCNTB0 = 15625;  //0.5s�ж�һ��
	
	/* ���س�ֵ������ʱ�� */
	TCON = (1<<1);   //�ֶ����£�Update TCNTB0 & TCMPB0

	/* ����Ϊ�Զ�����ģʽ */
	TCON &=~(1<<1);
	TCON |= (1<<0)|(1<<3);

	register_irq(10, timer_irq);	
}












