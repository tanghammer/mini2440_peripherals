CFLAGS += -g -Wall -O2 -march=armv4t -nostdlib

int.bin:
	arm-linux-gcc $(CFLAGS) -c head.S -o head.o
	arm-linux-gcc $(CFLAGS) -c main.c -o main.o
	arm-linux-ld -Tint.lds head.o main.o -o int_elf
	arm-linux-objcopy -O binary -S int_elf $@
	arm-linux-objdump -D -m arm int_elf > int.dis

clean:
	rm -f int.bin int_elf int.dis *.o		
