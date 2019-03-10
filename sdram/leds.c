#define	GPBCON		(*(volatile unsigned long *)0x56000010)
#define	GPBDAT		(*(volatile unsigned long *)0x56000014)

#define	GPB5OUT	    (0x01<<(5*2))
#define	GPB6OUT	    (0x01<<(6*2))
#define	GPB7OUT	    (0x01<<(7*2))
#define	GPB8OUT	    (0x01<<(8*2))

void  wait(volatile unsigned long dly)
{
	for(; dly > 0; dly--);
}

int main(void)
{   
    //设置GPB5-GPB8为输出
	GPBCON = GPB5OUT | GPB6OUT | GPB7OUT | GPB8OUT;

	while (1)
	{
	    GPBDAT = 0xFFFFFFFF;
	    wait(3000);
	    GPBDAT = 0x00000000;
	    wait(1000);
	}

	return 0;
}
