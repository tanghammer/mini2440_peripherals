# s3c2440调试nandflash裸机程序遇到的问题

按照前面sdram的代码，启动代码里面关看门狗、初始化存储控制器（主要是BANK0的Norflash和BANK6的SDRAM）、设置栈到SDRAM的最高地址，text段的数据直接从Norflash里面取。
![](https://i.imgur.com/7EWkcnq.png)

代码如下：
head.S
```asm
@*************************************************************************
@ File：head.S
@ 功能：设置SDRAM，将栈设置到SDRAM，然后继续执行
@*************************************************************************   

.equMEM_CTL_BASE,   0x48000000
.equSDRAM_BASE, 0x30000000

.text
.global _start
_start:
	b 		reset
	
reset:
bl  	disable_watch_dog   @ 关闭WATCHDOG，否则CPU会不断重启
bl  	memsetup@ 设置存储控制器
ldr 	sp, =0x34000000 @ 设置栈
bl  	main
halt_loop:
b   	halt_loop

disable_watch_dog:
	@ 往WATCHDOG寄存器写0即可
mov 	r1, #0x53000000
mov 	r2, #0x0
str 	r2, [r1]
mov 	pc, lr  				@ 返回

memsetup:
	@ 设置存储控制器以便使用SDRAM等外设
mov 	r1, #MEM_CTL_BASE   @ 存储控制器的13个寄存器的开始地址
adrlr2, 	mem_cfg_val @ 这13个值的起始存储地址
add 	r3, r1, #52 @ 13*4 = 54
@ 依次将内存控制单元对SDRAM初始化的参数写入至寄存器
str_loop:  
ldr 	r4, [r2], #4@ 读取设置值，并让r2加4
str 	r4, [r1], #4@ 将此值写入寄存器，并让r1加4
cmp 	r1, r3  @ 判断是否设置完所有13个寄存器
bne 	str_loop@ 若没有写成，继续
mov 	pc, lr  @ 返回


.align 4
mem_cfg_val:
@ 存储控制器13个寄存器的设置值
.long   0x22011110  @ BWSCON
.long   0x00000700  @ BANKCON0
.long   0x00000700  @ BANKCON1
.long   0x00000700  @ BANKCON2
.long   0x00000700  @ BANKCON3  
.long   0x00000700  @ BANKCON4
.long   0x00000700  @ BANKCON5
.long   0x00018005  @ BANKCON6
.long   0x00018005  @ BANKCON7
.long   0x008C07A3  @ REFRESH
.long   0x000000B1  @ BANKSIZE
.long   0x00000030  @ MRSRB6
.long   0x00000030  @ MRSRB7
```

nand.c
```c
#define LARGER_NAND_PAGE

#define GSTATUS1        (*(volatile unsigned int *)0x560000B0)
#define BUSY            1

#define NAND_SECTOR_SIZE    512
#define NAND_BLOCK_MASK     (NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP    2048
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

typedef unsigned int S3C24X0_REG32;


/* NAND FLASH (see S3C2410 manual chapter 6) */
typedef struct {
    S3C24X0_REG32   NFCONF;
    S3C24X0_REG32   NFCMD;
    S3C24X0_REG32   NFADDR;
    S3C24X0_REG32   NFDATA;
    S3C24X0_REG32   NFSTAT;
    S3C24X0_REG32   NFECC;
} S3C2410_NAND;

/* NAND FLASH (see S3C2440 manual chapter 6, www.100ask.net) */
typedef struct {
    S3C24X0_REG32   NFCONF;
    S3C24X0_REG32   NFCONT;
    S3C24X0_REG32   NFCMD;
    S3C24X0_REG32   NFADDR;
    S3C24X0_REG32   NFDATA;
    S3C24X0_REG32   NFMECCD0;
    S3C24X0_REG32   NFMECCD1;
    S3C24X0_REG32   NFSECCD;
    S3C24X0_REG32   NFSTAT;
    S3C24X0_REG32   NFESTAT0;
    S3C24X0_REG32   NFESTAT1;
    S3C24X0_REG32   NFMECC0;
    S3C24X0_REG32   NFMECC1;
    S3C24X0_REG32   NFSECC;
    S3C24X0_REG32   NFSBLK;
    S3C24X0_REG32   NFEBLK;
} S3C2440_NAND;


typedef struct {
    void (*nand_reset)(void);
    void (*wait_idle)(void);
    void (*nand_select_chip)(void);
    void (*nand_deselect_chip)(void);
    void (*write_cmd)(int cmd);
    void (*write_addr)(unsigned int addr);
    unsigned char (*read_data)(void);
}t_nand_chip;

static S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

static t_nand_chip nand_chip;

/* 供外部调用的函数 */
void nand_init(void);
void nand_read(unsigned char *buf, unsigned long start_addr, int size);

/* NAND Flash操作的总入口, 它们将调用S3C2440的相应函数 */
static void nand_reset(void);
static void wait_idle(void);
static void nand_select_chip(void);
static void nand_deselect_chip(void);
static void write_cmd(int cmd);
static void write_addr(unsigned int addr);
static unsigned char read_data(void);

/* S3C2440的NAND Flash处理函数 */
static void s3c2440_nand_reset(void);
static void s3c2440_wait_idle(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_write_cmd(int cmd);
static void s3c2440_write_addr(unsigned int addr);
static unsigned char s3c2440_read_data(void);

/* S3C2440的NAND Flash操作函数 */

/* 复位 */
static void s3c2440_nand_reset(void)
{
    s3c2440_nand_select_chip();
    s3c2440_write_cmd(0xff);  // 复位命令
    s3c2440_wait_idle();
    s3c2440_nand_deselect_chip();
}

/* 等待NAND Flash就绪 */
static void s3c2440_wait_idle(void)
{
    int i;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;
    while(!(*p & BUSY))
        for(i=0; i<10; i++);
}

/* 发出片选信号 */
static void s3c2440_nand_select_chip(void)
{
    int i;
    s3c2440nand->NFCONT &= ~(1<<1);
    for(i=0; i<10; i++);    
}

/* 取消片选信号 */
static void s3c2440_nand_deselect_chip(void)
{
    s3c2440nand->NFCONT |= (1<<1);
}

/* 发出命令 */
static void s3c2440_write_cmd(int cmd)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFCMD;
    *p = cmd;
}

/* 发出地址 */
static void s3c2440_write_addr(unsigned int addr)
{
    int i;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
    
    *p = addr & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 9) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 17) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 25) & 0xff;
    for(i=0; i<10; i++);
}


static void s3c2440_write_addr_lp(unsigned int addr)
{
	int i;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;
	
	*p = col & 0xff;			/* Column Address A0~A7 */
	for(i=0; i<10; i++);		
	*p = (col >> 8) & 0x0f; 	/* Column Address A8~A11 */
	for(i=0; i<10; i++);
	*p = page & 0xff;			/* Row Address A12~A19 */
	for(i=0; i<10; i++);
	*p = (page >> 8) & 0xff;	/* Row Address A20~A27 */
	for(i=0; i<10; i++);
	*p = (page >> 16) & 0x03;	/* Row Address A28~A29 */
	for(i=0; i<10; i++);
}


/* 读取数据 */
static unsigned char s3c2440_read_data(void)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFDATA;
    return *p;
}


/* 在第一次使用NAND Flash前，复位一下NAND Flash */
static void nand_reset(void)
{
    nand_chip.nand_reset();
}

static void wait_idle(void)
{
    nand_chip.wait_idle();
}

static void nand_select_chip(void)
{
    int i;
    nand_chip.nand_select_chip();
    for(i=0; i<10; i++);
}

static void nand_deselect_chip(void)
{
    nand_chip.nand_deselect_chip();
}

static void write_cmd(int cmd)
{
    nand_chip.write_cmd(cmd);
}
static void write_addr(unsigned int addr)
{
    nand_chip.write_addr(addr);
}

static unsigned char read_data(void)
{
    return nand_chip.read_data();
}


/* 初始化NAND Flash */
void nand_init(void)
{
#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

    nand_chip.nand_reset         = s3c2440_nand_reset;
    nand_chip.wait_idle          = s3c2440_wait_idle;
    nand_chip.nand_select_chip   = s3c2440_nand_select_chip;
    nand_chip.nand_deselect_chip = s3c2440_nand_deselect_chip;
    nand_chip.write_cmd          = s3c2440_write_cmd;
#ifdef LARGER_NAND_PAGE
    nand_chip.write_addr         = s3c2440_write_addr_lp;
#else
    nand_chip.write_addr		 = s3c2440_write_addr;
#endif
    nand_chip.read_data          = s3c2440_read_data;

    /* 设置时序 */
    s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
    /* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
    s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);
    
    /* 复位NAND Flash */
    nand_reset();
}


/* 读函数 */
void nand_read(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;

#ifdef LARGER_NAND_PAGE
    if ((start_addr & NAND_BLOCK_MASK_LP) || (size & NAND_BLOCK_MASK_LP)) {
        return ;    /* 地址或长度不对齐 */
    }
#else
    if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
        return ;    /* 地址或长度不对齐 */
    }
#endif	

    /* 选中芯片 */
    nand_select_chip();

    for(i=start_addr; i < (start_addr + size);) {
      /* 发出READ0命令 */
      write_cmd(0);

      /* Write Address */
      write_addr(i);
#ifdef LARGER_NAND_PAGE
      write_cmd(0x30);		
#endif
      wait_idle();

#ifdef LARGER_NAND_PAGE
      for(j=0; j < NAND_SECTOR_SIZE_LP; j++, i++) {
#else
	  for(j=0; j < NAND_SECTOR_SIZE; j++, i++) {
#endif
          *buf = read_data();
          buf++;
      }
    }

    /* 取消片选信号 */
    nand_deselect_chip();
    
    return ;
}

int main(void)
{
    unsigned int buffer;
    unsigned long nandPtr = 0x00000000;
    
    nand_init();

    while(1)
    {
        nand_read((unsigned char *)&buffer, nandPtr, 4);
        nandPtr = nandPtr + 4;
    }
    return 0;
}
```
Makefile
```makefile
CROSS_TOOLCHAIN := arm-linux

nand.bin : head.S  nand.c
	$(CROSS_TOOLCHAIN)-gcc -g -nostdlib -c -o head.o head.S
	$(CROSS_TOOLCHAIN)-gcc -g -nostdlib -c -o nand.o nand.c
	$(CROSS_TOOLCHAIN)-ld -Ttext 0x00000000 head.o nand.o -o nand_elf
	$(CROSS_TOOLCHAIN)-objcopy -O binary -S nand_elf nand.bin
	$(CROSS_TOOLCHAIN)-objdump -D -m arm  nand_elf > nand.dis
	
clean:
	rm -f nand.dis nand.bin nand_elf *.o
```
使用`arm-linux-gdb + JLinkGDBServer` （[如何使用arm-linux-gdb + JLinkGDBServer调试](https://github.com/tanghammer/mini2440_peripherals/blob/master/sdram/doc.md)） 在调试过程中，发现程序有复位现象。

![](https://i.imgur.com/DMU7U8y.png)

但是我已经在启动代码里面关闭了看门狗，看来是别的中断产生了，直接在查看当前寄存器状态`monitor regs`

![](https://i.imgur.com/kRFui7E.png)

CPSR寄存器显示CPU现在处于Undefined Mode。查看S3C2440数据手册

    OPERATING MODES
    ARM920T supports seven modes of operation:
    - User (usr): The normal ARM program execution state
    - FIQ (fiq): Designed to support a data transfer or channel process
    - IRQ (irq): Used for general-purpose interrupt handling 
    - Supervisor (svc): Protected mode for the operating system 
    - Abort mode (abt): Entered after a data or instruction prefetch abort 
    - System (sys): A privileged user mode for the operating system
    - Undefined (und): Entered when an undefined instruction is executed

未定义模式是指CPU执行了一条未定义的指令。那么就需要看到底是那条指令引起，所以得汇编级别的单步调试。重新加载程序，断点下在main里面

	load
	b main
	c
	set disassemble-next-line on
	si
	si
	...
	...
	si

![](https://i.imgur.com/QVmDFvv.png)

在执行了N条si后，发现当执行到nand_reset函数时，发生了复位。

![](https://i.imgur.com/6PuUx2q.png)

在执行到`0x000004b0`

	0x000004b0 <nand_reset+16>:  33 ff 2f e1     blx     r3

于是重新启动程序，在`0x000004b0`下断点`b *0x000004b0`，在并且打印r3的值

	Breakpoint 3, 0x000004b0 in nand_reset () at nand.c:173
	173         nand_chip.nand_reset();
	   0x000004a8 <nand_reset+8>:   08 30 9f e5     ldr     r3, [pc, #8]    ; 0x4b8 <nand_reset+24>
	   0x000004ac <nand_reset+12>:  00 30 93 e5     ldr     r3, [r3]
	=> 0x000004b0 <nand_reset+16>:  33 ff 2f e1     blx     r3
	(gdb) monitor reg r3
	Reading register (R3 = 0xFFFFFFFF)
	(gdb) 

发现R3居然为0xFFFFFFFF，blx到0xFFFFFFFF这个位置显然不对，那r3具体是什么意思呢?结合C代码

	static void nand_reset(void)
	{
	    nand_chip.nand_reset();
	}
查看反汇编nand.dis,nand_reset函数被翻译为

	000004a0 <nand_reset>:
	000004a0:	e92d4800 	push	{fp, lr}
	000004a4:	e28db004 	add	fp, sp, #4
	000004a8:	e59f3008 	ldr	r3, [pc, #8]	; 4b8 <nand_reset+0x18> ;取nand_chip地址（存在地址0x000004b8,值为0x0000878c），放到r3, r3 = &nand_chip
	000004ac:	e5933000 	ldr	r3, [r3]		;即ldr r3, [r3+0], nand_chip.nand_reset成员偏移为0，这句是取nand_chip.nand_reset函数指针给r3, r3 = nand_chip.nand_reset;
	000004b0:	e12fff33 	blx	r3	          ;调用nand_chip.nand_reset()函数
	000004b4:	e8bd8800 	pop	{fp, pc}
	000004b8:	0000878c 	andeq	r8, r0, ip, lsl #15	 ;这句不是一条汇编，而是一个数据，是nand_chip这个全局静态变量的地址

再看看nand_chip这个结构体各成员的值：

	(gdb) p nand_chip 
	$1 = {
	  nand_reset = 0xffffffff, 
	  wait_idle = 0xffffffff, 
	  nand_select_chip = 0xffffffff, 
	  nand_deselect_chip = 0xffffffff, 
	  write_cmd = 0xffffffff, 
	  write_addr = 0xffffffff, 
	  read_data = 0xffffffff
	}

**设置函数指针全失败。为什么呢？nand_chip这个全局变量的地址是0x000004b8，并且占了接下来的7*4个字节，赋值就是对这块内存区域进行写操作。0x000004b8这个地址是对应到NorFlash上的，而NorFlash是不能通过CPU地址和数据总线直接这么写的。在sdram实验时就说到了这点。可以这么理解：NorFlash如果在执行代码的时候，是只读的。因为Flash的特性是写操作要通过先擦除，再写的复杂时序，CPU单纯的发地址去写是实现不了的。**

那如何解决这个问题呢？如果代码在SDRAM里面运行就不会这样了，SDRAM是可以通过CPU地址和数据总线直接访问的，那么在启动代码里面把NorFlash里面的代码拷贝到SDRAM再在内存里面运行。修改head.s:

	reset:
	    bl  	disable_watch_dog               @ 关闭WATCHDOG，否则CPU会不断重启
	    bl  	memsetup                        @ 设置存储控制器
	    bl 		copy_norflash_to_sdram
	    ldr 	sp, =0x34000000                 @ 设置栈
	    bl  	main
	.........
	.........
	.........
	copy_norflash_to_sdram:
	    @ 将norflash数据全部复制到SDRAM中去
	    @ Nor启动, norflas起始地址0x00000000，SDRAM中起始地址0x30000000
	    mov r1, #0
	    ldr r2, =SDRAM_BASE
	    mov r3, #2*1024*1024	@Norflash大小
	lo:
	    ldr r4, [r1],#4     @ 从NorFlash读取4字节的数据，并让源地址4字节自增
	    str r4, [r2],#4     @ 将此4字节的数据复制到SDRAM中，并让目地地址4字节自增
	    cmp r1, r3          @ 判断是否完成：源地址等于NorFlash大小的未地址
	    bne 1b              @ 若没
	    mov	pc,     lo		@ 返回

增加一个将NorFlash内容全部拷贝至SDRAM的函数`copy_norflash_to_sdram`,Makefile作调整，将链接地址设置为0x30000000

	CROSS_TOOLCHAIN := arm-linux
	
	nand.bin : head.S  nand.c
		$(CROSS_TOOLCHAIN)-gcc -g -nostdlib -c -o head.o head.S
		$(CROSS_TOOLCHAIN)-gcc -g -nostdlib -c -o nand.o nand.c
		$(CROSS_TOOLCHAIN)-ld -Ttext 0x30000000 head.o nand.o -o nand_elf
		$(CROSS_TOOLCHAIN)-objcopy -O binary -S nand_elf nand.bin
		$(CROSS_TOOLCHAIN)-objdump -D -m arm  nand_elf > nand.dis
		
	clean:
		rm -f nand.dis nand.bin nand_elf *.o

这个时候再次调试

	Writing register (CPSR = 0x000000D3)
	Select auto target interface speed (8000 kHz)
	(gdb) load
	Loading section .text, size 0x7a8 lma 0x30000000
	Loading section .data, size 0x4 lma 0x300087a8
	Start address 0x30000000, load size 1964
	Transfer rate: 127 KB/sec, 982 bytes/write.
	(gdb) b main
	Breakpoint 1 at 0x30000778: file nand.c, line 285.
	(gdb) c
	Continuing.
	
	Breakpoint 1, main () at nand.c:285
	285         unsigned long nandPtr = 0x00000000;
	(gdb) n
	287         nand_init();
	(gdb) s
	nand_init () at nand.c:215
	215         nand_chip.nand_reset         = s3c2440_nand_reset;
	(gdb) s
	216         nand_chip.wait_idle          = s3c2440_wait_idle;
	(gdb) p nand_chip.nand_reset 
	$1 = (void (*)(void)) 0x300000b0 <s3c2440_nand_reset>
	(gdb) n
	217         nand_chip.nand_select_chip   = s3c2440_nand_select_chip;
	(gdb) n
	218         nand_chip.nand_deselect_chip = s3c2440_nand_deselect_chip;
	(gdb) n
	219         nand_chip.write_cmd          = s3c2440_write_cmd;
	(gdb) n
	221         nand_chip.write_addr         = s3c2440_write_addr_lp;
	(gdb) n
	225         nand_chip.read_data          = s3c2440_read_data;
	(gdb) n
	228         s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	(gdb) n
	230         s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);
	(gdb) n
	233         nand_reset();
	(gdb) p nand_chip 
	$2 = {
	  nand_reset = 0x300000b0 <s3c2440_nand_reset>, 
	  wait_idle = 0x300000d0 <s3c2440_wait_idle>, 
	  nand_select_chip = 0x3000013c <s3c2440_nand_select_chip>, 
	  nand_deselect_chip = 0x30000198 <s3c2440_nand_deselect_chip>, 
	  write_cmd = 0x300001cc <s3c2440_write_cmd>, 
	  write_addr = 0x30000318 <s3c2440_write_addr_lp>, 
	  read_data = 0x30000484 <s3c2440_read_data>
	}

可以看到nand_chip成员可以正常赋值了，nand_chip它的地址也在SDRAM所在的地址范围。继续汇编级的单步调试，
![](https://i.imgur.com/TctG3ty.png)

这个问题困扰了我，https://blog.csdn.net/smstong/article/details/53944794， 网上查到这个资料，原来s3c2440指令集里面不包含blx指令，s3c2440的数据手册也可以看出来，它只支持bx、bl、b、bxx(条件跳转)。这就解释的清楚了，`blx r3`，是由于blx的使用导致了Undefined Mode,blx指令的翻译是由arm-linux-gcc做的，那么只要指定我目标机器的指令集是s3c2440所属的指令集就行了，s3c2440是用的ARM920T核，指令集是ARMv4T。所以Makefile修改一下，指定指令集为armv4t：
```makefile
CROSS_TOOLCHAIN := arm-linux

nand.bin : head.S  nand.c
	$(CROSS_TOOLCHAIN)-gcc -march=armv4t -g -nostdlib -c -o head.o head.S
	$(CROSS_TOOLCHAIN)-gcc -march=armv4t -g -nostdlib -c -o nand.o nand.c
	$(CROSS_TOOLCHAIN)-ld -Ttext 0x30000000 head.o nand.o -o nand_elf
	$(CROSS_TOOLCHAIN)-objcopy -O binary -S nand_elf nand.bin
	$(CROSS_TOOLCHAIN)-objdump -D -m arm  nand_elf > nand.dis
	
clean:
	rm -f nand.dis nand.bin nand_elf *.o
```
再次编译，反汇编中代码段再也没有blx指令出现了，调试也正常了。单单一个nandflash的读操作就遇到了这么多问题，之前跟着网上的资料做，很多时候都帮我们避开了这些坑，或者说的时候都是一带而过。导致我们对一些知识点没有深刻的理解，所以我觉得要跟着比人做的同时，要从自己的理解角度再实现一遍。**其实这里对连接脚本，地址无关代码还需要有更深入的理解，后面再分析分析**。