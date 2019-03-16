###为什么第一个程序是操作SDRAM?
基于NorFlash启动的裸机程序如果要交换数据（使用栈），则必须借助SDRAM，NorFlash虽然是XIP，但是CPU是无法简单直接往里面写数据的，Flash类设备写数据必须要先擦除。要交换数据只能用内存SDRAM，所以要设置存储控制器。
![](https://i.imgur.com/7EWkcnq.png)


#linux上使用J-Link调试S3C2440裸机代码

##工具：
1. segger的jlink仿真器
1. segger的jlink for linux
1. 交叉编译工具链里面的arm-xx-linux-xx-gdb
1. 初始化脚本

##工具安装
###segger的jlink for linux
进入Segger官网--->Download--->J-Link/J-Trace--->J-Link Software and Documentation Pack。有deb、rpm、tgz三种包可供选择，随便选一个，我选的是tgz。由于我的仿真器是和谐版，所以我不敢选择高版本的J-Link，选择Older versions。下载一个比较老的，我下的是最老的。

![](https://i.imgur.com/trJ7ytO.png)

解压至你要安装的目录。里面有个README.txt，安装说明操作。安装完了为方便起见，记得export一下PATH。

###交叉编译工具链里面的arm-xx-linux-xx-gdb
这个不多介绍，你想自己编译就编译，想用别人制作好的也行。

###初始化脚本
此初始化脚本是给arm-xx-linux-xx-gdb用的，gdb可以通过`-x file.cmd`从文件里面指定命令序列。具体如下：
    #初始化 S3C2440 CPU.
    
    #定义一个函数(功能块)
    define reset_s3c2440
    	##设置小端模式，要和你跑的裸机程序一致.
    	monitor endian little
    	
    	##copied from JLinkGDBServer Document.
    	monitor reset 					#复位 S3C2440
    	monitor reg cpsr = 0xd3			#设置 cpsr 寄存器.
    	monitor speed auto				#Link Speed.
    end
    
    ##main function defined for connect to the TARGET.
    ##arg0 = IP地址.
    ##arg1 = JLinkGDBServer端口号.
    define connect_s3c2440
    	if $argc == 2
    	target remote $arg0:$arg1
    	reset_s3c2440
    	end
    end

大概就是通过IP:PORT连接上JLinkGDBServer,然后下发一系列的指令初始化s3c2440。这些指令也可以手动逐个敲进去。

##工作原理介绍
###参考[gdb工作原理](https://blog.csdn.net/u012658346/article/details/51159971)
![](https://i.imgur.com/e3g1lin.png)

本地调试搞linux都熟悉。有时候由于资源限制、或者目标机不在现场，就需要借助gdbserver做远程调试。可以这样理解，gdb是由gdb前端和gdb后台组成，gdb前端负责和用户交互，gdb后台负责和操作ptrace，在远程调试时gdbserver提供一个后台和一个通讯接口与本地的gdb前端交互。gdb和gdbserver之间可以通过串口，usb，tcp/ip来通讯。一般来说gdbserver远远小于gdb，所以资源限制情况下直接放一个gdbserver到目标机里面，然后远程调试。

###JLinkGDBServer
JLinkGDBServer又相当于把gdbserver---->ptrace---->target替换成了gdbserver---->jlink--->jlink调试器---->target。JLink还有好多好玩的工具J-Link Remote Server、J-Link Commander、J-Link RTT Viewer、J-Link SWO Viewer等等。


##调试步骤
###1.代码
https://github.com/tanghammer/mini2440_peripherals.git
###2.硬件连线
按照jlink仿真器接线方式，启动方式选择NorFlash。
###3.烧录程序
Windows下操作：
- 启动J-Flash
- file--->open project--->打开[s3c2440.jflash](https://pan.baidu.com/s/17QSzRdSOImYv3LM1UROqiw)
- file--->open data file--->选择要烧录的bin文件
- F7(Auto)

这样程序就烧录到了Norflash,下面是s3c2440.jflash分享链接。
链接：https://pan.baidu.com/s/17QSzRdSOImYv3LM1UROqiw 
提取码：vazg
###4.启动JLinkGDBServer
直接执行命令`JLinkGDBServer`
![](https://i.imgur.com/A3t7isd.png)
###5.启动GDB

    arm-none-linux-gnueabi-gdb -x s3c2440_init -ex "connect_s3c2440 192.168.20.156 2331" ./sdram_elf
###6.开始调试
    load
    b main
    c
![](https://i.imgur.com/z3z8Dz8.png)