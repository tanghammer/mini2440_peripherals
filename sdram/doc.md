###为什么第一个程序是操作SDRAM?
基于NorFlash启动的裸机程序如果要交换数据（使用栈），则必须借助SDRAM，NorFlash虽然是XIP，但是CPU是无法简单直接往里面写数据的，Flash类设备写数据必须要先擦除。要交换数据只能用内存SDRAM，所以要设置存储控制器。
![](https://i.imgur.com/7EWkcnq.png)