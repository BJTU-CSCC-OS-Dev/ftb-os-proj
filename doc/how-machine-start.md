注意：

* Risc-V的privilege是四层：M/H/S/U，没有“Kernel”。



 在QEMU下：

0. 一开始会令`PC`为`0x1000`，在这里会配置`$a0`为`$mhartid`，也就是CPU编号。

   并读取一系列地址，根据读出来的地址做`jr`。

   处于M-mode。

1. 跳转的地址是`0x8000_0000`，在编译的时候会通过把`entry.S`生成的`entry.o`放在`gcc`的第一个参数，让`_entry`这个label位于`0x8000_0000`。

   处于M-mode。

2. `_entry`里会做：

   设置`$sp = stack0 + (hartid * 4096)`，也就是设置一个内核栈，之后跳转到`start`。

   处于M-mode。

3. `start`位于`kernel/start.c`，会做：

   1. 设置`$mstatus.mpp`为S-mode。其中`mpp`指m-mode previous priviledge。
   2. 设置`$mepc`为`main`地址。
   3. 设置`$medeleg`和`$mideleg`为`0xFFFF`，这是为了让所有privilege mode下的trap都会进入supervisor mode。
   4. 设置`$sie`为`($sie | SIE_SEIE | SIE_STIE | SIE_SSIE)`。
   5. 按照SV39的写`$pmpaddr0`和`$pmpcfg0`。
   6. 调用`timerinit`来启动时钟中断。
   7. 通过`$mhartid`得到`hartid`，然后写入`$tp`（Thread Pointer）寄存器。
   8. 执行`mret`，跳转到`$mepc`的地址，并让CPU进入`$mpp`设置的模式。

   执行`mret`之前是M-mode，执行之后是S-mode。

4. 经过`mret`跳转到`main`函数，会做：

   对于0号CPU：

   1. 初始化`console`、输出debug信息。
   2. `kvminit`。

   
