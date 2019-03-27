#include "s3c24xx.h"

#define EINT8_23    5

/*
 * LED1--GPB5
 */
#define	GPB5_out	(1<<(5*2))
#define	GPB5_msk	(3<<(5*2))

/*
 * S1对应GPG0
 */
#define GPG0_eint     (0x2<<(0*2))
#define GPG0_msk    (3<<(0*2))


void init_led(void)
{
    // LED1,LED2,LED4对应的3根引脚设为输出
    GPBCON = GPB5_out;
    GPBDAT &= ~(0x1 << 5);
}

/*
 * 初始化GPIO引脚为外部中断
 * GPIO引脚用作外部中断时，默认为低电平触发、IRQ方式(不用设置INTMOD)
 */
void init_irq(void)
{
    // S1对应的引脚设为中断引脚 EINT
    GPGCON = GPG0_eint;

    EXTINT1 = 0x0A; //开启FLTEN8消抖，EINT8下降沿触发
    // 对于EINT8，需要在EINTMASK寄存器中使能它
    EINTMASK &= ~(1 << 8);
    /*
     * 设定优先级：
     * ARB_SEL0 = 00b, ARB_MODE0 = 0: REQ1 > REQ3，即EINT0 > EINT2
     * 仲裁器1、6无需设置
     * 最终：
     * EINT0 > EINT2 > EINT11即K2 > K3 > K4
     */
    //PRIORITY = (PRIORITY & ((~0x01) | (0x3 << 7))) | (0x0 << 7) ;
    
    //EINT8_23使能
    INTMSK   &=  (~(1 << EINT8_23));
}

void EINT_Handle()
{
    unsigned long oft = INTOFFSET;

    switch( oft )
    {
        // S2被按下
        case EINT8_23:
            {
                //GPB5 is on
                if ( GPBDAT & (0x1 << 5) )
                {
                    GPBDAT &= ~(0x1 << 5); //点亮LED1
                }
                else//GPB5 is off
                {
                    GPBDAT |= (0x1 << 5); //熄灭LED1
                }
                break;
            }

        default:
            break;
    }

    //清中断
    if( oft == EINT8_23 )
    {
        EINTPEND = (1 << 8);    // EINT8_23合用IRQ5
    }

    SRCPND = 1 << oft;
    INTPND = 1 << oft;
}

int main(void)
{
    init_led();
    init_irq();

    while(1);

    return 0;
}
