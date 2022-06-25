BOOTSEG = 0x07C0    /* original address of boot-sector */

    .code16
    .section ".bstext", "ax"
    .global _start

bootsect_start:
    # Normalize the start address
    ljmp    $BOOTSEG, $_start

_start:
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    xorw    %sp, %sp
    sti                 # 开中断
    cld                 # 清除方向标志

    movw    $loading_msg, %si
msg_loop:
    lodsb               # 从%si搬移字符到%al
    andb    %al, %al    # 要显示的字符
    jz  load_main       # 字符结束退出
    movb    $0xe, %ah   # 显示字符
    movw    $7, %bx     # 显示属性
    int $0x10           # 调用10H中断
    jmp msg_loop

load_main:
    xorw    %dx, %dx    # dl:磁盘(0表示软盘，80H表示硬盘), dh:磁头号
    movw    $2, %cx     # 低6位:扇区号, 高10位:磁道号
    movw    $512, %bx   # 读出数据存放地址
    movw    $0x210, %ax # ah:02H=读扇区, al:读扇区个数
    int $0x13           # 调用13H中断
    jnc load_ok         # CF=0则成功
    xorb    %dl, %dl    # dl:磁盘(0表示软盘，80H表示硬盘)
    xorb    %ah, %ah    # ah:00H=磁盘复位
    int $0x13           # 调用13H中断
    jmp load_main

load_ok:

# Zero the bss
	movw	$__bss_start, %di
	movw	$_end+3, %cx
	xorl	%eax, %eax
	subw	%di, %cx
	shrw	$2, %cx
	rep; stosl

# Jump to C code (should not return)
    calll   main

    .global die
    .type   die, @function
die:
    hlt
    jmp die
    .size   die, . - die

    .section ".bsdata", "a"
loading_msg:
    .ascii  "Loading system ...\r\n"
    .byte   0

    .section ".bsflag", "a"
boot_flag:  .word 0xAA55
