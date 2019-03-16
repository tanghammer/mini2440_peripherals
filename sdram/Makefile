CROSS_TOOLCHAIN := arm-none-linux-gnueabi

sdram.bin : head.S  leds.c
	$(CROSS_TOOLCHAIN)-gcc -g3 -nostdlib -c -o head.o head.S
	$(CROSS_TOOLCHAIN)-gcc -g3 -nostdlib -c -o leds.o leds.c
	$(CROSS_TOOLCHAIN)-ld -Ttext 0x00000000 head.o leds.o -o sdram_elf
	$(CROSS_TOOLCHAIN)-objcopy -O binary -S sdram_elf sdram.bin
	$(CROSS_TOOLCHAIN)-objdump -D -m arm  sdram_elf > sdram.dis
	
clean:
	rm -f sdram.dis sdram.bin sdram_elf *.o
