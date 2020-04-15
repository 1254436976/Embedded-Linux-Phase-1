/*
*测试平台:   ubuntu16.04(64位机器)  gcc -m32 -o push_test  push_test.c 
*编译平台：gcc
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
 *依据:x86平台,函数调用时参数传递是使用！堆栈！来实现的 
 *！！在调用push_test函数时，第一个参数的类型应为char *类型，后面的参数为可变变量
 =======================================
 *！！在使用va_arg(ap,t)时，t为char变量编译时会出现：so you should pass ‘int’ not ‘char’ to ‘va_arg’。
 *修改方式：char change int ,因为要保证四个字节对齐
 =======================================
 *目的:将所有传入的参数全部打印出来 
 *调试后得到 format address : 0xffc16d50
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
	/*指针操作：（1）移动指针，（2）取值*/
	//p = p+sizeof(char *);
	//one = *((struct person *)p);
	one = va_arg(p,struct person);
	printf("name = %s, age = %d, class = %c,id = %d\n",one.name,one.age,one.class,one.id);
	
	/*指针操作：（1）移动指针，（2）取值*/	
	//p = p+sizeof(struct person);
	//dnum = *((double *)p);
	dnum = va_arg(p,double);
	printf(" arg2 = %f\n", dnum);

	/*指针操作：（1）移动指针，（2）取值*/	
	//p = p+sizeof(double);
	//inum = *((int *)p);
	inum = va_arg(p,int);
	printf("arg3 = %d\n",inum);

	/*指针操作：（1）移动指针，（2）取值*/	
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


