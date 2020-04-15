/*
*����ƽ̨:   ubuntu16.04(64λ����)  gcc -m32 -o push_test  push_test.c 
*����ƽ̨��gcc
*/

#include <stdio.h>
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
 *Ŀ��:�����д���Ĳ���ȫ����ӡ���� 
 *���Ժ�õ� format address : 0xffc16d50
 */ 
int push_test(const char *format, ...)
{
	char *p = (char *)&format;
	struct person one;
	double dnum;
	int inum;
	char c;
	
	printf("arg1 data: %s  \n",format);

	/*ָ���������1���ƶ�ָ�룬��2��ȡֵ*/
	p = p+sizeof(char *);
	one = *((struct person *)p);
	printf("name = %s, age = %d, class = %c,id = %d\n",one.name,one.age,one.class,one.id);
	
	/*ָ���������1���ƶ�ָ�룬��2��ȡֵ*/	
	p = p+sizeof(struct person);
	dnum = *((double *)p);
	printf(" arg2 = %f\n", dnum);

	p = p+sizeof(double);
	inum = *((int *)p);
	printf("arg3 = %d\n",inum);

	p = p+((sizeof(char)+3) & ~3);
	c = *((char *)p);
	printf("arg3 = %c\n",c);
	
	p = (char *)0;
	
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


