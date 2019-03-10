sdram.bin : head.S  leds.c
	arm-none-linux-gnueabi-gcc  -nostdlib -c -o head.o head.S
	arm-none-linux-gnueabi-gcc -nostdlib -c -o leds.o leds.c
	arm-none-linux-gnueabi-ld -Ttext 0x30000000 head.o leds.o -o sdram_elf
	arm-none-linux-gnueabi-objcopy -O binary -S sdram_elf sdram.bin
	arm-none-linux-gnueabi-objdump -D -m arm  sdram_elf > sdram.dis
clean:
	rm -f   sdram.dis sdram.bin sdram_elf *.o
