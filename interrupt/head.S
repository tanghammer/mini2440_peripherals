@******************************************************************************
@ File：head.S
@ 功能：初始化，设置中断模式、管理模式的栈，设置好中断处理函数
@******************************************************************************       

.equ        MEM_CTL_BASE,       0x48000000
.equ        SDRAM_BASE,         0x30000000

.extern     main
.extern     EINT_Handle

@ 定义一个段，存储异常向量表
    .section "vectors"
start:
@******************************************************************************       
@ 中断向量，本程序中，除Reset和HandleIRQ外，其它异常都没有使用
@******************************************************************************       
    b   Reset

@ 0x04: 未定义指令中止模式的向量地址
HandleUndef:
    b   HandleUndef 
 
@ 0x08: 管理模式的向量地址，通过SWI指令进入此模式
HandleSWI:
    b   HandleSWI

@ 0x0c: 指令预取终止导致的异常的向量地址
HandlePrefetchAbort:
    b   HandlePrefetchAbort

@ 0x10: 数据访问终止导致的异常的向量地址
HandleDataAbort:
    b   HandleDataAbort

@ 0x14: 保留
HandleNotUsed:
    b   HandleNotUsed

@ 0x18: 中断模式的向量地址
    b   HandleIRQ

@ 0x1c: 快中断模式的向量地址
HandleFIQ:
    b   HandleFIQ

@ 代码段
    .text 
Reset:
    bl  	disable_watch_dog               @ 关闭WATCHDOG，否则CPU会不断重启
    bl  	memsetup                        @ 设置存储控制器
    bl 		copy_norflash_to_sdram
        
    msr cpsr_c, #0xD3       @ 进入管理模式
    ldr sp, =0x34000000     @ 设置管理模式栈指针，
                            @ 其实复位之后，CPU就处于管理模式，
                            @ 前面的“ldr sp, =0x34000000”完成同样的功能，此句可省略
                            
    msr cpsr_c, #0xD2       @ 进入中断模式
    ldr sp, =0x33FFE000     @ 设置中断模式栈指针，与管理模式SP相隔8K空间
    
    msr cpsr_c, #0x5F       @ 设置I-bit=0，开IRQ中断
    
    ldr lr, =halt_loop      @ 设置返回地址
    ldr pc, =main           @ 调用main函数
halt_loop:
    b   halt_loop

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

copy_norflash_to_sdram:
        ldr   r2, =data_size
        cmp   r2, #0x0              @若数据段大小为0，则直接返回，不做拷贝动作
        moveq pc, lr
        ldr   r0, =flash_sdata      @数据段在flash里面的起始地址
        ldr   r1, =ram_sdata        @ram起始地址
copy:
        ldrb  r4, [r0], #1
        strb  r4, [r1], #1
        subs  r2, r2, #1
        bne   copy
        mov	pc,     lr      @ 返回

HandleIRQ:
    sub lr, lr, #4                  @ 计算返回地址
    stmdb   sp!,    { r0-r12,lr }   @ 保存使用到的寄存器
                                    @ 注意，此时的sp是中断模式的sp
                                    @ 初始值是上面设置的3072
    
    ldr lr, =int_return             @ 设置调用ISR即EINT_Handle函数后的返回地址  
    ldr pc, =EINT_Handle            @ 调用中断服务函数，在interrupt.c中
int_return:
    ldmia   sp!,    { r0-r12,pc }^  @ 中断返回, ^表示将spsr的值复制到cpsr

