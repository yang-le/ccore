BOOTSEG = 0x07C0    /* original address of boot-sector */

    .code16
    .global _start

_start:
    # Normalize the start address
    ljmp    $BOOTSEG, $start

start:
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es
	movw    %ax, %ss
	xorw    %sp, %sp
	sti                 # 开中断
	cld                 # 清除方向标志

    movw    $hello_msg, %si
    call    show_msg

load_ccore:
    xorw    %dx, %dx    # dl:磁盘(0表示软盘，80H表示硬盘), dh:磁头号
    movw    $2, %cx     # 低6位:扇区号, 高10位:磁道号
    movw    $512, %bx   # 读出数据存放地址
    movw    $0x204, %ax # ah:02H=读扇区, al:读扇区个数
    int $0x13           # 调用13H中断
    jnc load_ok         # CF=0则成功
    xorb    %dl, %dl    # dl:磁盘(0表示软盘，80H表示硬盘)
    xorb    %ah, %ah    # ah:00H=磁盘复位
    int $0x13           # 调用13H中断
    jmp load_ccore

load_ok:
    calll   ccore

    movw    $bye_msg, %si
    call    show_msg

die:
    hlt
    jmp die

show_msg:
    lodsb               # 从%si搬移字符到%al
    andb    %al, %al    # 要显示的字符
    jz  show_msg_end    # 字符结束退出
    movb    $0xe, %ah   # 显示字符
    movw    $7, %bx     # 显示属性
    int $0x10           # 调用10H中断
    jmp show_msg
show_msg_end:
    ret

hello_msg:
    .ascii  "\r\nLoading system ...\r\n"
    .byte   0

bye_msg:
    .ascii  "\r\nYou should never go here.\r\n"
    .byte   0

    .org 510
    .word 0xAA55
