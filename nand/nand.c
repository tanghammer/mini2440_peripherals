#define BUSY            1
#define LARGER_NAND_PAGE

#define NAND_SECTOR_SIZE    512
#define NAND_BLOCK_MASK     (NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP    2048
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)


/*NANDFLASH控制器基址*/
#define S3C24X0NAND_BASE    (0x4E000000)

/*通用状态寄存器*/
#define GSTATUS0_BASE       (0x560000AC)
#define GSTATUS1_BASE       (0x560000B0)

typedef unsigned int S3C24X0_REG32;

/*NAND flash configuration regiser*/
typedef struct nfconf_reg
{
    volatile unsigned int BusWidth:1;        /*  [0]        NAND flash memory I/O bus width for auto-booting AdvFlash PageSize
                                                        0:8-bit,    1:16-bit
                                                    This bit is determined by GPG15 pin status during reset and wake-up from sleep mode.
                                    */
    volatile unsigned int AddrCycle:1;       /*  [1]       NAND flash memory Address cycle for auto-booting AdvFlash PageSize(read only)
                                                    When AdvFlash is 0:
                                                        0:3 address cycle,    1:4 address cycle
                                                    When AdvFlash is 1:
                                                        0:4 address cycle,    1:5 address cycle
                                                    This bit is determined by GPG14 pin status during reset and wake-up from sleep mode.
                                    */

    volatile unsigned int PageSize:1;        /*  [2]       NAND flash memory page size for auto-booting AdvFlash PageSize(read only)
                                                    When AdvFlash is 0:
                                                        0:256 Word/page,    1:512 Bytes/page
                                                    When AdvFlash is 1:
                                                        0:1024 Word/page,   1:2048 Bytes/page
                                                    This bit is determined by GPG13 pin status during reset and wake-up from sleep mode.
                                    */
                                    
    volatile unsigned int AdvFlash:1;        /*  [3]       Advance NAND flash memory for auto-booting(read only)
                                                        0:Support 256 or 512 byte/page NAND flash memory
                                                        1:Support 1024 or 2048 byte/page NAND flash memory
                                                    This bit is determined by NCON0 pin status during reset and wake-up from sleep mode.
                                    */

    volatile unsigned int Twrph1:3;          /*  [4:6]     TWRPH1 duration setting value(0~7),Duration = HCLK x (TWRPH1 + 1) */
    volatile unsigned int Reserved3:1;       /*  [7]       unused                                                            */
    volatile unsigned int Twrph0:3;          /*  [8:10]    TWRPH0 duration setting value(0~7),Duration = HCLK x (TWRPH0 + 1) */
    volatile unsigned int Reserved2:1;       /*  [11]      unused                                                            */
    volatile unsigned int TaclsDuration:2;   /*  [12:13]   CLE & ALE duration setting value(0~3), Duration = HCLK x TACLS    */
    volatile unsigned int Reserved1:2;       /*  [14:15]   unused                                                            */
    volatile unsigned int Reserved0:16;      /*  [16:31]   unused                                                            */
}nfconf_reg_ts;

/*NAND flash control regiser*/
typedef struct nfcont_reg
{
    volatile unsigned int Mode:1;                /*  [0]     NAND flash controller operating mode
                                                                0: NAND flash controller disable (Don’t work)
                                                                1: NAND flash controller enable                     */

    volatile unsigned int Reg_nCE:1;             /*  [1]     NAND Flash Memory nFCE signal control
                                                                0: Force nFCE to low (Enable chip select)
                                                                1: Force nFCE to high (Disable chip select)
                                                     Note:  During boot time, it is controlled automatically.
                                                     This value is only valid while MODE bit is 1                   */
    volatile unsigned int Reserved4:2;           /*  [2:3]    unused                                                */
    volatile unsigned int InitEcc:1;             /*  [4]     Initialize ECC decoder/encoder(Write-only)
                                                                1: Initialize ECC decoder/encoder                   */

    volatile unsigned int MainEccLock:1;         /*  [5]     Lock Main data area ECC generation
                                                                0: Unlock main data area ECC generation
                                                                1: Lock main data area ECC generation
                                                         Main area ECC status register is NFMECC0/1
                                                         (0x4E00002C/30)                                                */
    volatile unsigned int SpareEccLock:1;        /*  [6]     Lock spare area ECC generation.
                                                                0: Unlock spare ECC    
                                                                1: Lock spare ECC
                                                         Spare area ECC status register is FSECC(0x4E000034)             */

    volatile unsigned int Reserved3:1;           /*  [7]    unused                                                   */
    volatile unsigned int RnB_TransMode:1;       /*  [8]     RnB transition detection configuration
                                                0: Detect rising edge    
                                                1: Detect falling edge                                      */
    volatile unsigned int EnbRnBINT:1;           /*  [9]     RnB status input signal transition interrupt control
                                                0: Disable RnB interrupt 
                                                1: Enable RnB interrupt                                     */
    volatile unsigned int EnblllegalAccINT:1;    /*  [10]    Illegal access interrupt control
                                                0: Disable interrupt    
                                                1: Enable interrupt
                                            Illegal access interrupt is occurred when CPU tries to
                                            program or erase locking area (the area setting in
                                            NFSBLK(0x4E000038) to NFEBLK(0x4E00003C)-1).                    */
    volatile unsigned int Reserved2:1;           /*  [11]    unused                                                  */
    volatile unsigned int SoftLock:1;            /*  [12]    Soft Lock configuration
                                                    0: Disable lock      1: Enable lock
                                                Soft lock area can be modified at any time by software.
                                                When it is set to 1, the area setting in
                                                NFSBLK(0x4E000038) to NFEBLK(0x4E00003C)-1 is
                                                unlocked, and except this area, write or erase command
                                                will be invalid and only read command is valid. 
                                                When you try to write or erase locked area, the illegal
                                                access will be occur (NFSTAT[3] bit will be set).
                                                If the NFSBLK and NFEBLK are same, entire area will be
                                                locked.                                                         */
    volatile unsigned int LockTight:1;           /*  [13]    Lock-tight configuration
                                                0: Disable lock-tight    
                                                1: Enable lock-tight,
                                            Once this bit is set to 1, you cannot clear. Only reset or
                                            wake up from sleep mode can make this bit disable(can
                                            not cleared by software).
                                            When it is set to 1, the area setting in
                                            NFSBLK(0x4E000038) to NFEBLK(0x4E00003C)-1 is
                                            unlocked, and except this area, write or erase command
                                            will be invalid and only read command is valid. 
                                            When you try to write or erase locked area, the illegal
                                            access will be occur (NFSTAT[3] bit will be set).
                                            If the NFSBLK and NFEBLK are same, entire area will be
                                            locked.                                                         */
    volatile unsigned int Reserved1:2;           /*  [14:15]   unused                                                */
    volatile unsigned int Reserved0:16;          /*  [16:31]   unused                                                */
}nfcont_reg_ts;

/*NAND flash command set regiser*/
typedef struct nfcmmd_reg
{
    volatile unsigned char NFcmmd;          /*  [0:7]     NAND flash memory command value                            */
    volatile unsigned char Reserved1;       /*  [8:15]    unused                                                     */
    volatile unsigned short Reserved0;      /*  [16:31]   unused                                                     */
}nfcmmd_reg_ts;

/*NAND flash address set regiser*/
typedef struct nfaddr_reg
{
    volatile unsigned char NFaddr;          /*  [0:7]     NAND flash memory address value                            */
    volatile unsigned char Reserved1;       /*  [8:15]    unused                                                     */
    volatile unsigned short Reserved0;      /*  [16:31]   unused                                                     */
}nfaddr_reg_ts;

/*NAND flash address set regiser*/
typedef struct nfdatar_reg
{
    volatile unsigned int NFdata;            /*  NAND flash read/program data value for I/O                          */        
}nfdata_reg_ts;

/*NAND flash ECC regiser*/
typedef struct nfmeccd_reg
{
    volatile unsigned char EccData0_0;
    volatile unsigned char EccData0_1;
    volatile unsigned char EccData1_0;
    volatile unsigned char EccData1_1;
}nfmeccd_reg_ts;

/*NAND flash ECC operation status register*/
typedef struct nfstat_reg
{
    volatile unsigned int RnB:1;             /* The satus of RnB input pin.(read-only)
                                                    0:NAND Flash memory busy
                                                    1:NAND Flash memory ready to operate    */
    volatile unsigned int nCE:1;             /* The satus of nCE output pin.(read-only)     */
    volatile unsigned int RnB_TransDetect:1;
    volatile unsigned int IiiegalAccess:1;
    volatile unsigned int Reserved2:3;
    volatile unsigned int Reserved1:1;
    volatile unsigned int Reserved0:24;
}nfstat_reg_ts;

/*NAND flash ECC Status register*/
typedef struct nfestat_reg
{
    volatile unsigned int MainError:2;       /*  Indicates whether main data area bit fail error occurred
                                                        00: No Error    01: 1-bit error(correctable)
                                                        10: Multiple error  11: ECC area error               */
    volatile unsigned int SpareError:2;      /*  Indicates whether spare area bit fail error occurred
                                                        00: No Error    01: 1-bit error(correctable)
                                                        10: Multiple error  11: ECC area error               */
    volatile unsigned int MErrorBitNo:3;     /*  In main data area, Indicates which bit is error         */
    volatile unsigned int MErrorDataNo:11;   /*  In main data area, Indicates which number data is error */
    volatile unsigned int SErrorBitNo:3;     /*  In spare area, Indicates which bit is error*/
    volatile unsigned int SErrorDataNo:4;    /*  In spare area, Indicates which number data is error     */

    volatile unsigned int Reserved0:7;       /*  [25:31]   unused                                        */ 
}nfestat_reg_ts;

/*NAND flash ECC register for data*/
typedef struct nfmecc_reg
{
    volatile unsigned char MECC0;            /*  ECC0 for data[7:0]  */
    volatile unsigned char MECC1;            /*  ECC1 for data[7:0]  */
    volatile unsigned char MECC2;            /*  ECC2 for data[7:0]  */
    volatile unsigned char MECC3;            /*  ECC3 for data[7:0]  */
}nfmecc_reg_ts;

/*NAND flash ECC register for I/O[15:0]*/
typedef struct nfsecc_reg
{
    volatile unsigned char SECC0_0;          /*  Spare area ECC0 status for I/O[7:0]  */
    volatile unsigned char SECC0_1;          /*  Spare area ECC1 status for I/O[7:0]  */
    volatile unsigned char SECC1_0;          /*  Spare area ECC0 status for I/O[15:8]   */
    volatile unsigned char SECC1_1;          /*  Spare area ECC1 status for I/O[15:8]   */
}nfsecc_reg_ts;

typedef struct nfsnblk_reg
{
    volatile unsigned char Blk_Addr0;        /*  The 1st block address of the block erase operation.Only bit [7:5] are valid   */
    volatile unsigned char Blk_Addr1;        /*  The 2nd block address of the block erase operation   */
    volatile unsigned char Blk_Addr2;        /*  The 3nd block address of the block erase operation   */
    volatile unsigned char Reserved;
}nfsnblk_reg_ts;

/* NAND FLASH 控制寄存器组*/
typedef struct {
    nfconf_reg_ts   NFCONF;
    nfcont_reg_ts   NFCONT;
    nfcmmd_reg_ts   NFCMD;
    nfaddr_reg_ts   NFADDR;
    nfdata_reg_ts   NFDATA;
    nfmeccd_reg_ts  NFMECCD0;
    nfmeccd_reg_ts  NFMECCD1;
    nfmeccd_reg_ts  NFSECCD;
    nfstat_reg_ts   NFSTAT;
    nfestat_reg_ts  NFESTAT0;
    nfestat_reg_ts  NFESTAT1;
    nfmecc_reg_ts   NFMECC0;
    nfmecc_reg_ts   NFMECC1;
    nfsecc_reg_ts   NFSECC;
    nfsnblk_reg_ts  NFSBLK;
    nfsnblk_reg_ts  NFEBLK;
} S3C2440_NAND;

/*NAND Flash操作接口*/
typedef struct {
    void (*nand_reset)(void);
    void (*wait_ready)(void);
    void (*nand_select_chip)(void);
    void (*nand_deselect_chip)(void);
    void (*write_cmd)(int cmd);
    void (*write_addr)(unsigned int addr);
    unsigned char (*read_data)(void);
}t_nand_chip;


static volatile S3C2440_NAND* s3c2440nand = (S3C2440_NAND *)(S3C24X0NAND_BASE);
static t_nand_chip nand_chip;

/* NAND Flash操作的总入口, 它们将调用S3C2440的相应函数 */
static void nand_reset(void);
static void wait_ready(void);
static void nand_select_chip(void);
static void nand_deselect_chip(void);
static void write_cmd(int cmd);
static void write_addr(unsigned int addr);
static unsigned char read_data(void);

/* S3C2440的NAND Flash处理函数 */
static void s3c2440_nand_reset(void);
static void s3c2440_wait_ready(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_write_cmd(int cmd);
static void s3c2440_write_addr(unsigned int addr);
static unsigned char s3c2440_read_data(void);


/* S3C2440的NAND Flash操作函数 */
/* 复位 
     ______                        _______
nCE        \______________________/
                 _____
I/OX ___________/ FFH \___________________
                \_____/
RnB  ___________              ____________
                \____________/
*/
static void s3c2440_nand_reset(void)
{
    s3c2440_nand_select_chip();
    s3c2440_write_cmd(0xFF);            // 复位命令
    s3c2440_wait_ready();
    s3c2440_nand_deselect_chip();
}


//==============================================================================
/* 等待NAND Flash就绪 */
static void s3c2440_wait_ready(void)
{
    int i;

    //wait NAND Flash memory ready to operate
    while( !(s3c2440nand->NFSTAT.RnB) )
    {
        for(i=0; i<10; i++);
    }    
}

/* 发出片选信号 */
static void s3c2440_nand_select_chip(void)
{
    int i;
    
    s3c2440nand->NFCONT.Reg_nCE = 0;    //Force nFCE to low (Enable chip select)
    
    for(i=0; i<10; i++)                 //延时
    {
        ;                
    }
}

/* 取消片选信号 */
static void s3c2440_nand_deselect_chip(void)
{
    s3c2440nand->NFCONT.Reg_nCE = 1;    //Force nFCE to high (Disable chip select)
}

/* 发出命令 */
static void s3c2440_write_cmd(int cmd)
{
    s3c2440nand->NFCMD.NFcmmd = cmd;
}

/* 发出地址 */
static void s3c2440_write_addr(unsigned int addr)
{
    int i;
    
    s3c2440nand->NFADDR.NFaddr = addr & 0xFF;
    for(i=0; i<10; i++);
    
    s3c2440nand->NFADDR.NFaddr = (addr >> 9) & 0xFF;
    for(i=0; i<10; i++);
    
    s3c2440nand->NFADDR.NFaddr = (addr >> 17) & 0xFF;
    for(i=0; i<10; i++);
    
    s3c2440nand->NFADDR.NFaddr = (addr >> 25) & 0xFF;
    for(i=0; i<10; i++);
}


static void s3c2440_write_addr_lp(unsigned int addr)
{
	int i;
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;
	
	s3c2440nand->NFADDR.NFaddr = col & 0xFF;			/* Column Address A0~A7 */
	for(i=0; i<10; i++);
	
	s3c2440nand->NFADDR.NFaddr = (col >> 8) & 0x0F; 	/* Column Address A8~A11 */
	for(i=0; i<10; i++);
	
	s3c2440nand->NFADDR.NFaddr = page & 0xFF;			/* Row Address A12~A19 */
	for(i=0; i<10; i++);
	
	s3c2440nand->NFADDR.NFaddr = (page >> 8) & 0xFF;	/* Row Address A20~A27 */
	for(i=0; i<10; i++);
	
	s3c2440nand->NFADDR.NFaddr = (page >> 16) & 0x03;	/* Row Address A28~A29 */
	for(i=0; i<10; i++);
}


/* 读取数据 */
static unsigned char s3c2440_read_data(void)
{
    return s3c2440nand->NFDATA.NFdata;
}

/* 在第一次使用NAND Flash前，复位一下NAND Flash */
static void nand_reset(void)
{
    nand_chip.nand_reset();
}

static void wait_ready(void)
{
    nand_chip.wait_ready();
}

static void nand_select_chip(void)
{
    int i;
    
    nand_chip.nand_select_chip();
    
    for(i=0; i<10; i++)
    {
    }
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
    nand_chip.wait_ready         = s3c2440_wait_ready;
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
    s3c2440nand->NFCONF.TaclsDuration = TACLS;
    s3c2440nand->NFCONF.Twrph0 = TWRPH0;
    s3c2440nand->NFCONF.Twrph1 = TWRPH1;
    
    /* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
    //s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);
    s3c2440nand->NFCONT.InitEcc = 1;    //Initialize ECC decoder/encoder 
    s3c2440nand->NFCONT.Reg_nCE = 1;    //Force nFCE to high (Disable chip select) 
    s3c2440nand->NFCONT.Mode = 1;       //NAND flash controller enable
    
    /* 复位NAND Flash */
    nand_reset();
}

/* 读函数 */
void nand_read(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;

#ifdef LARGER_NAND_PAGE
    if ((start_addr & NAND_BLOCK_MASK_LP) || (size & NAND_BLOCK_MASK_LP)) 
    {
        return ;    /* 地址或长度不对齐 */
    }
#else
    if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) 
    {
        return ;    /* 地址或长度不对齐 */
    }
#endif	

    /* 选中芯片 */
    nand_select_chip();

    for(i=start_addr; i < (start_addr + size);) 
    {
        /* 发出READ0命令 */
        write_cmd(0);

        /* Write Address */
        write_addr(i);
#ifdef LARGER_NAND_PAGE
        write_cmd(0x30);		
#endif
        wait_ready();

#ifdef LARGER_NAND_PAGE
        for(j=0; j < NAND_SECTOR_SIZE_LP; j++, i++) 
        {
#else
        for(j=0; j < NAND_SECTOR_SIZE; j++, i++) 
        {
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
    unsigned long nandPtr;
    
    nand_init();

    nandPtr = 0;
    while(1)
    {
        nand_read(((unsigned char*)&buffer), nandPtr, 0x4);
        nandPtr += 4;
    }
    
	return 0;
}
