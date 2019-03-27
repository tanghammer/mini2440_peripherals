CFLAGS += -g3 -Wall -march=armv4t -nostdlib

NAME = int

$(NAME).bin:head.o main.o Makefile
	arm-linux-ld -T int.lds head.o main.o -o $(NAME).elf
	arm-linux-objcopy -O binary -S $(NAME).elf $@
	arm-linux-objdump -D -m arm $(NAME).elf > $(NAME).dis
	arm-linux-objdump -D -b binary -m arm $(NAME).bin> $(NAME).asm
	
head.o:head.S
	arm-linux-gcc $(CFLAGS) -c head.S -o head.o

main.o:main.c
	arm-linux-gcc $(CFLAGS) -c main.c -o main.o

clean:
	rm -f $(NAME).bin $(NAME).elf $(NAME).dis *.o $(NAME).asm
