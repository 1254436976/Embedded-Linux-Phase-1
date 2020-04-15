/*
*测试平台:   ubuntu16.04(64位机器)  gcc -m32 -o push_test  push_test.c 
*编译平台：gcc
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
 *依据:x86平台,函数调用时参数传递是使用！堆栈！来实现的 
 *！！在调用push_test函数时，第一个参数的类型应为char *类型，后面的参数为可变变量
 *目的:将所有传入的参数全部打印出来 
 *调试后得到 format address : 0xffc16d50
 */ 
int push_test(const char *format, ...)
{
	char *p = (char *)&format;
	struct person one;
	double dnum;
	int inum;
	char c;
	
	printf("arg1 data: %s  \n",format);

	/*指针操作：（1）移动指针，（2）取值*/
	p = p+sizeof(char *);
	one = *((struct person *)p);
	printf("name = %s, age = %d, class = %c,id = %d\n",one.name,one.age,one.class,one.id);
	
	/*指针操作：（1）移动指针，（2）取值*/	
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


