
#include "s3c2440_soc.h"
#include "uart.h"

int main (void)
{
	unsigned char s;	
	uart0_init();
	puts("Hey Man!\n\r");

	while(1)
	{
		s=getchar();
		if (s == '\r')
		{
			putchar('\n');
		}
		
		if (s == '\n')
		{
			putchar('\r');
		}
		
		putchar(s);
	}
		
}
