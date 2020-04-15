#include "s3c2440_soc.h"
#include "uart.h"
#include "sdram_init.h"

char g_Char = 'A';
char g_Char2 = 'a';
char i ='0';


int main(void)
{
	unsigned char c;
	int flag;

	//interrupt_init();
	key_eint_int();
	timer0_init();
	uart0_init();
	sdram_init();
	
	puts("uart0 init success!\n\r''");
	putchar(i);
	
	while(1)
	{
		putchar(g_Char); 
		g_Char++;
		delay(1000000);
		putchar(g_Char2); 
		g_Char2++;
		delay(1000000);
	}
	
	
	
	return 0;
}



