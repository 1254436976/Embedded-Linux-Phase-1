/*
*����ƽ̨:   ubuntu16.04(64λ����)  gcc -m32 -o push_test  push_test.c 
*����ƽ̨��gcc
*/

#include <stdio.h>
#include <stdarg.h>
/*
typedef char *  va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )
*/



struct person {
	char *name;
	int   age;
	char  class;
	int   id;
};

/* 
 *int printf(const char *format, ...); 
 *����:x86ƽ̨,��������ʱ����������ʹ�ã���ջ����ʵ�ֵ� 
 *�����ڵ���push_test����ʱ����һ������������ӦΪchar *���ͣ�����Ĳ���Ϊ�ɱ����
 =======================================
 *������ʹ��va_arg(ap,t)ʱ��tΪchar��������ʱ����֣�so you should pass ��int�� not ��char�� to ��va_arg����
 *�޸ķ�ʽ��char change int ,��ΪҪ��֤�ĸ��ֽڶ���
 =======================================
 *Ŀ��:�����д���Ĳ���ȫ����ӡ���� 
 *���Ժ�õ� format address : 0xffc16d50
 */ 
int push_test(const char *format, ...)
{
//	char *p = (char *)&format;
	va_list p;
	va_start(p,format);
	struct person one;
	double dnum;
	int inum;
	char c;
	
	printf("arg1 data: %s  \n",format);
	/*ָ���������1���ƶ�ָ�룬��2��ȡֵ*/
	//p = p+sizeof(char *);
	//one = *((struct person *)p);
	one = va_arg(p,struct person);
	printf("name = %s, age = %d, class = %c,id = %d\n",one.name,one.age,one.class,one.id);
	
	/*ָ���������1���ƶ�ָ�룬��2��ȡֵ*/	
	//p = p+sizeof(struct person);
	//dnum = *((double *)p);
	dnum = va_arg(p,double);
	printf(" arg2 = %f\n", dnum);

	/*ָ���������1���ƶ�ָ�룬��2��ȡֵ*/	
	//p = p+sizeof(double);
	//inum = *((int *)p);
	inum = va_arg(p,int);
	printf("arg3 = %d\n",inum);

	/*ָ���������1���ƶ�ָ�룬��2��ȡֵ*/	
	//p = p+((sizeof(char)+3) & ~3);
	//c = *((char *)p);
	c = va_arg(p,int);
	printf("arg3 = %c\n",c);
	
	va_end(p);
	
	return 0;
}

int main(int argc,char **argv)
{
	struct person one ={"jane",16,'a',123};

	
    //push_test("abcd");
    //push_test("abcd",123);	 
    //push_test("abcd",123,per);  				
    //push_test("abcd",123,per,'c');  	 
    push_test("ssss",one,2.78,123,'c'); 	
			
	return 0;
}	


