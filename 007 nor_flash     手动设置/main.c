
#include "s3c2440_soc.h"
#include "uart.h"
#include "my_printf.h"
#include "init.h"
#include "led.h"

int main(void)
{
	unsigned char c;
	
	uart0_init();
	puts("Please input a num\n\r");
	
	while(1)
	{
		c = getchar();
		if (c == '\r')
		{
			putchar('\n');
		}

		if (c == '\n')
		{
			putchar('\r');
		}
		putchar(c);

		if(c >'0' && c<'7' )
		{
		bank0_init(c - '0');
		led_test();
		}
		else
		{
			puts("Error,c should between 0-7\n\r");
			puts("Please input a num\n\r");
		
		}
	}
	return 0;
}



