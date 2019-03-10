#define	GPBCON		(*(volatile unsigned long *)0x56000010)
#define	GPBDAT		(*(volatile unsigned long *)0x56000014)

#define	GPB5_out	(0x01<<(5*2))
#define	GPB6_out	(0x01<<(6*2))
#define	GPB7_out	(0x01<<(7*2))
#define	GPB8_out	(0x01<<(8*2))

void  wait(volatile unsigned long dly)
{
	for(; dly > 0; dly--);
}

int main(void)
{   
	int i;
	int b;

	GPBCON = GPB5_out | GPB6_out | GPB7_out | GPB8_out;

	while (1)
	{
		for (i=0; i < 300; i++)
		{
			b = 300 - i;
			
			GPBDAT = 0 << 5 | 0 << 6 | 0 << 7 | 0 << 8;
			wait(b);
			GPBDAT = 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;
			wait(i);
		}
	}

	return 0;
}
