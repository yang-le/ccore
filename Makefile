CC = clang
CXX = clang++
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-i386

REALMODE_CFLAGS = -m16 -g -Os -march=i386 -mregparm=3 -fno-stack-protector -nostdinc

OFLAGS = -O binary --strip-all
ASFLAGS = $(REALMODE_CFLAGS) -Iinclude
CXXFLAGS = $(REALMODE_CFLAGS) -Iinclude
LFLAGS = -melf_i386 -nostdlib -Map system.map --script

TARGET = ccore
ALL_OBJS = boot.o main.o bioscall.o tty.o regs.o copy.o memory.o printf.o string.o a20.o pm.o pmjump.o

all: linker.ld $(ALL_OBJS)
	$(LD) -o $(TARGET) $(LFLAGS) $^

img: all
	$(OBJCOPY) $(OFLAGS) $(TARGET) $(TARGET).img

# img: bin
# 	cp $(TARGET).bin $(TARGET).img
# 	dd if=/dev/zero of=$(TARGET).img bs=1k seek=1440 count=0

qemu: img
	$(QEMU) -nographic -fda $(TARGET).img

debug: img
	$(QEMU) -nographic -fda $(TARGET).img -S -s

clean:
	rm *.o $(TARGET) $(TARGET).img system.map
