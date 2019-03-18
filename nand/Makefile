CROSS_TOOLCHAIN := arm-linux

nand.bin : head.S  nand.c
	$(CROSS_TOOLCHAIN)-gcc -march=armv4t -g -nostdlib -c -o head.o head.S
	$(CROSS_TOOLCHAIN)-gcc -march=armv4t -g -nostdlib -c -o nand.o nand.c
	$(CROSS_TOOLCHAIN)-ld -Ttext 0x30000000 head.o nand.o -o nand_elf
	$(CROSS_TOOLCHAIN)-objcopy -O binary -S nand_elf nand.bin
	$(CROSS_TOOLCHAIN)-objdump -D -m arm  nand_elf > nand.dis
	
clean:
	rm -f nand.dis nand.bin nand_elf *.o
