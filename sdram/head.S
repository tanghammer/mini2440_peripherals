@*************************************************************************
@ File：head.S
@ 功能：设置SDRAM，将栈设置到SDRAM，然后继续执行
@*************************************************************************       

.equ        MEM_CTL_BASE,       0x48000000
.equ        SDRAM_BASE,         0x30000000

.text
.global _start
_start:
    bl  	disable_watch_dog               @ 关闭WATCHDOG，否则CPU会不断重启
    bl  	memsetup                        @ 设置存储控制器
    ldr 	sp, =0x34000000                 @ 设置栈
    bl  	main
halt_loop:
    b   	halt_loop

disable_watch_dog:
	@ 往WATCHDOG寄存器写0即可
    mov 	r1,     #0x53000000
    mov 	r2,     #0x0
    str 	r2,     [r1]
    mov 	pc,     lr      				@ 返回

memsetup:
	@ 设置存储控制器以便使用SDRAM等外设
    mov 	r1,     #MEM_CTL_BASE       @ 存储控制器的13个寄存器的开始地址
    adrl    r2, 	mem_cfg_val         @ 这13个值的起始存储地址
    add 	r3,     r1, #52             @ 13*4 = 54
    @ 依次将内存控制单元对SDRAM初始化的参数写入至寄存器
str_loop:  
    ldr 	r4,     [r2], #4            @ 读取设置值，并让r2加4
    str 	r4,     [r1], #4            @ 将此值写入寄存器，并让r1加4
    cmp 	r1,     r3                  @ 判断是否设置完所有13个寄存器
    bne 	str_loop                    @ 若没有写成，继续
    mov 	pc,     lr                  @ 返回


.align 4
mem_cfg_val:
    @ 存储控制器13个寄存器的设置值
    .long   0x22011110      @ BWSCON
    .long   0x00000700      @ BANKCON0
    .long   0x00000700      @ BANKCON1
    .long   0x00000700      @ BANKCON2
    .long   0x00000700      @ BANKCON3  
    .long   0x00000700      @ BANKCON4
    .long   0x00000700      @ BANKCON5
    .long   0x00018005      @ BANKCON6
    .long   0x00018005      @ BANKCON7
    .long   0x008C07A3      @ REFRESH
    .long   0x000000B1      @ BANKSIZE
    .long   0x00000030      @ MRSRB6
    .long   0x00000030      @ MRSRB7

