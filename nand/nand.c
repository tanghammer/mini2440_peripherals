#define BUSY            1
#define LARGER_NAND_PAGE

#define NAND_SECTOR_SIZE    512
#define NAND_BLOCK_MASK     (NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP    2048
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)
#define NAND_OOB_SIZE_LP       64

/*NANDFLASH控制器基址*/
#define S3C24X0NAND_BASE    (0x4E000000)


/*命令*/
#define CMD_READ1                0x00               //页读命令周期1
#define CMD_READ2                0x30               //页读命令周期2
#define CMD_READID               0x90               //读ID命令
#define CMD_WRITE1               0x80               //页写命令周期1
#define CMD_WRITE2               0x10               //页写命令周期2
#define CMD_ERASE1               0x60               //块擦除命令周期1
#define CMD_ERASE2               0xD0               //块擦除命令周期2
#define CMD_STATUS               0x70               //读状态命令
#define CMD_RESET                0xFF               //复位
#define CMD_RANDOMREAD1          0x05               //随意读命令周期1
#define CMD_RANDOMREAD2          0xE0               //随意读命令周期2
#define CMD_RANDOMWRITE          0x85               //随意写命令


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

static volatile S3C2440_NAND* s3c2440nand = (S3C2440_NAND *)(S3C24X0NAND_BASE);

/* NAND Flash操作的总入口, 它们将调用S3C2440的相应函数 */


/* S3C2440的NAND Flash处理函数 */
static void s3c2440_nand_reset(void);
static void s3c2440_wait_ready(void);
static void s3c2440_clear_ready(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_write_cmd(unsigned char cmd);
static void s3c2440_write_addr_byte(unsigned char addr);
static void s3c2440_write_addr(unsigned int addr);
static unsigned char s3c2440_read_data8(void);
static unsigned short s3c2440_read_data16(void);
static unsigned int s3c2440_read_data32(void);
static void s3c2440_write_data8(unsigned char data);
static void s3c2440_write_data16(unsigned short data);
static void s3c2440_write_data32(unsigned int data);


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
    s3c2440_nand_select_chip();         //片选
    s3c2440_clear_ready();              //清空RnB
    s3c2440_write_cmd(CMD_RESET);       //复位命令
    s3c2440_wait_ready();               //等待ready
    s3c2440_nand_deselect_chip();       //关片选
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

/* 清空RnB忙信号*/
static void s3c2440_clear_ready(void)
{
    s3c2440nand->NFSTAT.RnB = 1;    
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
static void s3c2440_write_cmd(unsigned char cmd)
{
    s3c2440nand->NFCMD.NFcmmd = cmd;
}

static void s3c2440_write_addr_byte(unsigned char addr)
{
    s3c2440nand->NFADDR.NFaddr = addr;
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
static unsigned char s3c2440_read_data8(void)
{
    return s3c2440nand->NFDATA.NFdata & 0x000000FF;
}

static unsigned short s3c2440_read_data16(void)
{
    return s3c2440nand->NFDATA.NFdata & 0x0000FFFF;
}

static unsigned int s3c2440_read_data32(void)
{
    return s3c2440nand->NFDATA.NFdata;
}

/* 写入数据 */
static void s3c2440_write_data8(unsigned char data)
{
    s3c2440nand->NFDATA.NFdata = 0xFFFFFF00 | data;
}

/* 写入数据 */
static void s3c2440_write_data16(unsigned short data)
{
    s3c2440nand->NFDATA.NFdata = 0xFFFF0000 | data;
}

/* 写入数据 */
static void s3c2440_write_data32(unsigned int data)
{
    s3c2440nand->NFDATA.NFdata = data;
}


/* 初始化NAND Flash */
void nand_init(void)
{
#define TACLS   0
#define TWRPH0  4
#define TWRPH1  2

    /* 设置时序 */
    s3c2440nand->NFCONF.TaclsDuration = TACLS;
    s3c2440nand->NFCONF.Twrph0 = TWRPH0;
    s3c2440nand->NFCONF.Twrph1 = TWRPH1;
    
    /*非锁定，屏蔽nandflash中断，初始化ECC及锁定main区和spare区ECC，使能nandflash片选及控制器 */
    s3c2440nand->NFCONT.LockTight = 0;
    s3c2440nand->NFCONT.SoftLock = 0;
    s3c2440nand->NFCONT.EnblllegalAccINT = 0;
    s3c2440nand->NFCONT.EnbRnBINT = 0;
    s3c2440nand->NFCONT.RnB_TransMode = 0;
    s3c2440nand->NFCONT.SpareEccLock = 1;
    s3c2440nand->NFCONT.MainEccLock = 1;
    s3c2440nand->NFCONT.InitEcc = 1;    //Initialize ECC decoder/encoder 
    s3c2440nand->NFCONT.Reg_nCE = 1;    //Force nFCE to high (Disable chip select) 
    s3c2440nand->NFCONT.Mode = 1;       //NAND flash controller enable
    
    /* 复位NAND Flash */
    s3c2440_nand_reset();
}

unsigned char nand_readid(void)
{
    int i;
    unsigned char pMID;
    unsigned char pDID;
    unsigned char cyc3, cyc4, cyc5;
    unsigned int dat;

    s3c2440_nand_select_chip();         //打开nandflash片选
    s3c2440_clear_ready();              //清RnB信号
    s3c2440_write_cmd(CMD_READID);      //读ID命令
    s3c2440_write_addr_byte(0x0);       //写0x00地址
    s3c2440_wait_ready();               //等待就绪

    //读五个周期的ID
    dat = s3c2440_read_data32();        //厂商ID：0xEC
    cyc4 = (dat>>24) & 0xFF;
    cyc3 = (dat>>16) & 0xFF;
    pDID = (dat>>8) & 0xFF;
    pMID= dat & 0xFF;
    cyc5 = s3c2440_read_data8();        //0x44
    s3c2440_nand_deselect_chip();       //关闭nandflash片选

    return (pDID);

}

void nand_readpage(unsigned int page)
{
    unsigned int i, mecc0, secc;
    unsigned char buf[NAND_SECTOR_SIZE_LP + NAND_OOB_SIZE_LP];
    //NF_RSTECC();                //复位ECC
    //NF_MECC_UnLock();           //解锁main区ECC

    s3c2440_nand_select_chip();             //使能芯片 
    s3c2440_clear_ready();                  //清除RnB
    s3c2440_write_cmd(CMD_READ1);           //页读命令周期1，0x00

    //写入5个地址周期
    s3c2440_write_addr_byte(0x00);                   //列地址A0-A7
    s3c2440_write_addr_byte(0x00);                   //列地址A8-A11
    s3c2440_write_addr_byte((page) & 0xFF);          //行地址A12-A19
    s3c2440_write_addr_byte((page >> 8) & 0xFF);     //行地址A20-A27
    s3c2440_write_addr_byte((page >> 16) & 0xFF);    //行地址A28
 
    s3c2440_write_cmd(CMD_READ2);               //页读命令周期2，0x30
    s3c2440_wait_ready();                       //等待RnB信号变高，即不忙

    for (i = 0; i < NAND_SECTOR_SIZE_LP + NAND_OOB_SIZE_LP; i=i+4)
    {
        *((unsigned int *)(buf+i)) = s3c2440_read_data32();
    }

#if 0
    //NF_MECC_Lock();          //锁定main区ECC值
    //NF_SECC_UnLock();        //解锁spare区ECC

    //mecc0=NF_RDDATA();   //读spare区的前4个地址内容，即第2048~2051地址，这4个字节为main区的ECC

    //把读取到的main区的ECC校验码放入NFMECCD0/1的相应位置内
    //rNFMECCD0=((mecc0&0xff00)<<8)|(mecc0&0xff);
    //rNFMECCD1=((mecc0&0xff000000)>>8)|((mecc0&0xff0000)>>16);
                
    //NF_SECC_Lock();       //锁定spare区的ECC值
    //secc=NF_RDDATA();   //继续读spare区的4个地址内容，即第2052~2055地址，其中前2个字节为spare区的ECC值
    
    //rNFSECCD=((secc&0xff00)<<8)|(secc&0xff);//把读取到的spare区的ECC校验码放入NFSECCD的相应位置内
#endif
    s3c2440_nand_deselect_chip();   //关闭nandflash片选

#if 0
    //判断所读取到的数据是否正确
    if ((rNFESTAT0&0xf) == 0x0)
    {
        return 0x66;                  //正确
    }
    else
    {
        return 0x44;                  //错误
    }
#endif
}

void nand_writepage(unsigned int  page_number)
{
    unsigned int i, mecc0, secc;
    unsigned char stat, temp;

#if 0
    temp = rNF_IsBadBlock(page_number>>6);     //判断该块是否为坏块
    if(temp == 0x33)
    {
        return 0x42;       //是坏块，返回
    }
#endif

#if 0
    NF_RSTECC();          //复位ECC
    NF_MECC_UnLock();     //解锁main区的ECC
#endif

    s3c2440_nand_select_chip();          //打开nandflash片选
    s3c2440_clear_ready();        //清RnB信号

    s3c2440_write_cmd(CMD_WRITE1);           //页写命令周期1

    //写入5个地址周期
    s3c2440_write_addr_byte(0x00);           //列地址A0~A7
    s3c2440_write_addr_byte(0x00);           //列地址A8~A11
    s3c2440_write_addr_byte((page_number) & 0xff);         //行地址A12~A19
    s3c2440_write_addr_byte((page_number >> 8) & 0xff);    //行地址A20~A27
    s3c2440_write_addr_byte((page_number >> 16) & 0xff);  //行地址A28

    for (i = 0; i < NAND_SECTOR_SIZE_LP + NAND_OOB_SIZE_LP; i++)//写入一页数据
    {
        s3c2440_write_data32(0xDDDDDDDD);
    }

#if 0
    NF_MECC_Lock();    //锁定main区的ECC值
    mecc0=rNFMECC0;    //读取main区的ECC校验码

    //把ECC校验码由字型转换为字节型，并保存到全局变量数组ECCBuf中
    ECCBuf[0]=(U8)(mecc0&0xff);
    ECCBuf[1]=(U8)((mecc0>>8) & 0xff);
    ECCBuf[2]=(U8)((mecc0>>16) & 0xff);
    ECCBuf[3]=(U8)((mecc0>>24) & 0xff);
    NF_SECC_UnLock();                  //解锁spare区的ECC

    //把main区的ECC值写入到spare区的前4个字节地址内，即第2048~2051地址
    for(i=0;i<4;i++)
    {
        s3c2440_write_data8(ECCBuf[i]);
    }

    NF_SECC_Lock();                      //锁定spare区的ECC值
    secc=rNFSECC;                   //读取spare区的ECC校验码

    //把ECC校验码保存到全局变量数组ECCBuf中
    ECCBuf[4]=(U8)(secc&0xff);
    ECCBuf[5]=(U8)((secc>>8) & 0xff);

    //把spare区的ECC值继续写入到spare区的第2052~2053地址内
    for(i=4;i<6;i++)
    {
        NF_WRDATA8(ECCBuf[i]);
    }
#endif

    s3c2440_write_cmd(CMD_WRITE2);  //页写命令周期2
    
    s3c2440_wait_ready();
    
    s3c2440_write_cmd(CMD_STATUS);          //读状态命令
    //判断状态值的第6位是否为1，即是否在忙，该语句的作用与NF_DETECT_RB();相同
    do
    {
        stat = s3c2440_read_data8();
    }while(!(stat&0x40));

    s3c2440_nand_deselect_chip();                    //关闭Nand Flash片选

    //判断状态值的第0位是否为0，为0则写操作正确，否则错误
    if (stat & 0x1)
    {
#if 0
        temp = rNF_MarkBadBlock(page_number>>6);//标注该页所在的块为坏块
        if (temp == 0x21)
        {
            return 0x43            //标注坏块失败
        }
        else
#endif
        {
            temp = 2;
            //return 0x44;           //写操作失败
        }
    }
    else
    {
        temp = 0;
        //return 0x66;                  //写操作成功
    }
}

void nand_eraseblock(unsigned int block_number)
{
    char stat, temp;

#if 0
    temp = rNF_IsBadBlock(block_number);     //判断该块是否为坏块
    if(temp == 0x33)
    {
        return 0x42;           //是坏块，返回
    }
#endif

    s3c2440_nand_select_chip();       //打开nandflash片选
    s3c2440_clear_ready();            //清RnB信号

    s3c2440_write_cmd(CMD_ERASE1);    //擦除命令周期1

    //写入3个地址周期，从A18开始写起
    s3c2440_write_addr_byte((block_number << 6) & 0xff);         //行地址A18~A19
    s3c2440_write_addr_byte((block_number >> 2) & 0xff);         //行地址A20~A27
    s3c2440_write_addr_byte((block_number >> 10) & 0xff);        //行地址A28
    s3c2440_write_cmd(CMD_ERASE2);                               //擦除命令周期2

    //s3c2440_wait_ready();          //延时一段时间
    
    s3c2440_write_cmd(CMD_STATUS);          //读状态命令
    //判断状态值的第6位是否为1，即是否在忙，该语句的作用与s3c2440_wait_ready();相同
    do
    {
        stat = s3c2440_read_data8();
    }while(!(stat&0x40));

    s3c2440_nand_deselect_chip();             //关闭Nand Flash片选

    //判断状态值的第0位是否为0，为0则擦除操作正确，否则错误
    if (stat & 0x1)
    {
#if 0
        temp = rNF_MarkBadBlock(page_number>>6);    //标注该块为坏块

        if (temp == 0x21)
        {
            return 0x43            //标注坏块失败
        }
        else
#endif
        {
            //return 0x44;           //擦除操作失败
            temp = 1;
        }
    }
    else
    {
        //return 0x66;                  //擦除操作成功
        temp = 0;
    }
}

unsigned char nand_ramdomread(unsigned int page_number, unsigned int add)
{
    unsigned char dat;
    
    s3c2440_nand_select_chip();     //打开Nand Flash片选
    s3c2440_clear_ready();          //清RnB信号

    s3c2440_write_cmd(CMD_READ1);           //页读命令周期1

    //写入5个地址周期
    s3c2440_write_addr_byte(0x00);                      //列地址A0~A7
    s3c2440_write_addr_byte(0x00);                      //列地址A8~A11
    s3c2440_write_addr_byte((page_number) & 0xFF);      //行地址A12~A19
    s3c2440_write_addr_byte((page_number >> 8) & 0xFF); //行地址A20~A27
    s3c2440_write_addr_byte((page_number >> 16) & 0xFF);//行地址A28

    s3c2440_write_cmd(CMD_READ2);          //页读命令周期2
    s3c2440_wait_ready();                   //等待RnB信号变高，即不忙

    
    s3c2440_write_cmd(CMD_RANDOMREAD1);     //随机读命令周期1
    //页内地址
    s3c2440_write_addr_byte((char)(add&0xFF));              //列地址A0~A7
    s3c2440_write_addr_byte((char)((add>>8)&0x0F));         //列地址A8~A11
    s3c2440_write_cmd(CMD_RANDOMREAD2);                     //随机读命令周期2
    dat = s3c2440_read_data8();             //读取数据

    s3c2440_nand_deselect_chip();
    return dat;                            
}

void nand_ramdomwrite(unsigned int page_number, unsigned int add, unsigned char dat)
{
    unsigned char temp,stat;
    
    s3c2440_nand_select_chip();     //打开Nand Flash片选
    s3c2440_clear_ready();          //清RnB信号

    s3c2440_write_cmd(CMD_WRITE1);  //页写命令周期1
    //写入5个地址周期
    s3c2440_write_addr_byte(0x00);                          //列地址A0~A7
    s3c2440_write_addr_byte(0x00);                          //列地址A8~A11
    s3c2440_write_addr_byte((page_number) & 0xFF);          //行地址A12~A19
    s3c2440_write_addr_byte((page_number >> 8) & 0xFF);     //行地址A20~A27
    s3c2440_write_addr_byte((page_number >> 16) & 0xFF);    //行地址A28
    
    s3c2440_write_cmd(CMD_RANDOMWRITE);                     //随意写命令
    //页内地址
    s3c2440_write_addr_byte((char)(add&0xFF));              //列地址A0~A7
    s3c2440_write_addr_byte((char)((add>>8)&0x0F));         //列地址A8~A11
    s3c2440_write_data8(dat);                               //写入数据
    s3c2440_write_cmd(CMD_WRITE2);                          //页写命令周期2 
    s3c2440_wait_ready();                                   //延时一段时间 

    s3c2440_write_cmd(CMD_STATUS);      //读状态命令
    //判断状态值的第6位是否为1，即是否在忙，该语句的作用与NF_DETECT_RB();相同
    do
    {
           stat = s3c2440_read_data8();
    }while(!(stat&0x40));

    s3c2440_nand_deselect_chip();                    //关闭Nand Flash片选

    //判断状态值的第0位是否为0，为0则写操作正确，否则错误
    if (stat & 0x1)
    {
        temp = 1;
        //return 0x44;                  //失败
    }
    else
    {
        temp = 0;
        //return 0x66;                  //成功
    }
}

int main(void)
{    
    nand_init();
    nand_readid();
    nand_eraseblock(0);
    nand_writepage(0);
    nand_readpage(0);
    nand_ramdomwrite(1, 1, 0xAC);
    nand_ramdomread(1, 1);
    nand_readpage(1);
    
	return 0;
}
