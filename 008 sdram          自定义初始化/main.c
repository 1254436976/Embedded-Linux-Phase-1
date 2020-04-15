
#include "s3c2440_soc.h"
#include "uart.h"
#include "my_printf.h"
#include "sdram_init.h"
#include "led.h"

int main(void)
{
	unsigned char c;
	int flag;
	
	uart0_init();
	sdram_init();
	puts("Uart0 text success!\n\r");

	flag=sdram_test();

	if(flag == 0)
	{
		while(1)
		{
			puts("Sdram init success!\n\r");
			led_test();
			
	    }
	}
	else
		puts("Sdram init failure!\n\r");
	
	return 0;
}



