#include "s3c2440_soc.h"

typedef void(*irq_fp)(int);  //����irq_fpΪָ������ָ�����ͣ��ú���û�з�������ֵ,��(int)����//
irq_fp irq_handle[32];

/* EINTPEND����ͨ�����˼Ĵ���֪��EINT4-EINT23�ĸ������жϣ����ж�ʱ��������д1 */

/* SRCPND ������ʾ�ĸ��жϲ�����, ��Ҫ�����Ӧλ
 * EINT0-BIT[0]
 * EINT2-BIT[2]
 * EINR8_23-BIT[5]
 */

/* INTMOD �������÷����ж�ģʽ��IRQ��FIQ��
 * 0-IRQ��1-FIQ��Ĭ��Ϊ0
 */

/* INTMSK ���������ж�, 1-masked
 * EINT0-BIT[0]
 * EINT2-BIT[2]
 * EINR8_23-BIT[5]
 */
 /* ��ʼ���жϿ����� */


//	void interrupt_init(void)
//	{
//	INTMSK &=~ ((1<<0)|(1<<2)|(1<<5)); 	
//	INTMSK &= ~(1<<10);  /* enable timer0 int */
//	}

 

/* INTPND ������ʾ��ǰ���ȼ���ߵġ����ڷ������ж�, ��Ҫ�����Ӧλ
 * EINT0-BIT[0]
 * EINT2-BIT[2]
 * EINR8_23-BIT[5]
 */
 
/* INTOFFSET ������ʾINTPND����һλ������Ϊ1
 * ���λ����ͨ�����SRCPND��INTPND���Զ������
 */




/* ����һ��ע�ắ�������������жϺź��жϵ��ú��� */
void register_irq(int irq, irq_fp fp)
{
	irq_handle[irq] = fp;
	INTMSK &=~ (1<<irq);	
}

/* �жϴ�����
 * �ȷֱ��ж�Դ���Բ�ͬ���ж�Դ���д���
 * �����ж�
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
			/* �ɿ� */
			GPFDAT |= (1<<6);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<6);
		}

	}
	else if (flag == 2)
	{
		if (valf & (1<<2)) /* s3 --> gpf5 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<5);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<5);
		}
	}
	else if (flag == 5)
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (valg & (1<<3)) /* s4 --> gpf4 */
			{
				/* �ɿ� */
				GPFDAT |= (1<<4);
			}
			else
			{
				/* ���� */
				GPFDAT &= ~(1<<4);
			}
		}
		else if (val & (1<<19)) /* eint19 */
		{
			if (valg & (1<<11))
			{
				/* �ɿ� */
				/* Ϩ������LED */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* ����: ��������LED */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}	
	EINTPEND = val;
}


/* 
 *��ʼ����������������Ϊ�ж�Դ
 *S2:EINT0:GPF0 S3:EINT2:GPF2 S4:EINT11:GPG3 S5:EINT19:GPG11
 *�����жϷ�ʽ��˫�ߴ���
 */
void key_eint_int(void)
{
	/* 
	*���ð���
	*���üĴ����������㡢������
	*/
	GPFCON &=~((3<<0)|(3<<4));
	GPFCON |= ((2<<0)|(2<<4)); //����S2��S3

	GPGCON &=~((3<<6)|(3<<22));
	GPGCON |= ((2<<22)|(2<<6)); //����S4��S5

	EXTINT0 |= ((7<<0)|(7<<8)); //���� EINT0��EINT2

	EXTINT1 |= (7<<12); //����EINT11

	EXTINT2 |= (7<<12); //����EINT19

	EINTMASK &=~ ((1<<11)|(1<<19)); //ʹ��EINT11,19

	/*
	*����LED��
	*����GPF4��5��6Ϊ�����
	*�ȶԼĴ���CPFCON��0
	*�������üĴ�������GPF4,5,6Ϊ�����
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


