CC = clang++
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-x86_64

OFLAGS = --strip-all -O binary
CFLAGS = -nostdinc #-m64
LFLAGS = -nostdlib -Map system.map --script linker.ld #-m elf_x86_64

TARGET = ccore

all: boot.o main.o
	$(LD) $(LFLAGS) -o $(TARGET) $^
	
bin: all
	$(OBJCOPY) $(OFLAGS) $(TARGET) $(TARGET).bin

img: bin
	cp $(TARGET).bin $(TARGET).img
	dd if=/dev/zero of=$(TARGET).img bs=1k seek=1440 count=0

qemu: img
	$(QEMU) -nographic -fda $(TARGET).img

clean:
	rm *.o $(TARGET) $(TARGET).bin $(TARGET).img system.map

%.o : %.cc
	$(CC) $(CFLAGS) -o $@ -c $^

%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -o $@ -c $^
