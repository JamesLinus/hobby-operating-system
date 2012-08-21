SOURCES=boot.o main.o kio.o time.o gdt.o gdt_asm.o idt.o idt_asm.o pit.o util.o paging.o pmm.o vmm.o tasks.o hdd.o fs/vfs.o fs/fat32.o
CFLAGS=-std=c99 -m32 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf
CC=i586-elf-gcc

all: $(SOURCES) link

%.c:+	
	$(CC) $(CFLAGS) -o $@ $<

link:
	i586-elf-ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

clean:
	rm -f *.o
	rm -f loader.o kernel.o kernel.bin
	
install:
	cp kernel A:\boot\kernel
