
#include "s3c2440_soc.h"

void bank0_init(int data)
{
	BANKCON0 = (data << 8); 
}


