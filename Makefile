# Конфигурация
CC := gcc
ASM := nasm
LD = ld
QEMU := qemu-system-x86_64

# Флаги
ASMFLAGS := -f bin
CFLAGS := -O0 -nostdlib -ffreestanding -fno-pie -fno-pic \
		  -mno-red-zone -m32 -march=i386 -mtune=i386
LDFLAGS := -m elf_i386 -T linker.ld -nostdlib --oformat binary

# Исходные файлы
BOOT_SRC := boot.asm
STAGE2_SRC := stage2.asm
KERNEL_ENTRY := kernel_entry.asm
KERNEL_SRCS := kernel.c $(wildcard *.c)
KERNEL_OBJS := kernel_entry.o $(KERNEL_SRCS:.c=.o)

# Цели по умолчанию
all: disk.img

# Создание образа диска
disk.img: boot.bin stage2.bin kernel.bin text.txt
	dd if=/dev/zero of=$@ bs=512 count=2880
	mkfs.fat -F 12 $@

	dd if=boot.bin of=$@ seek=0 conv=notrunc
	mcopy -i $@ stage2.bin ::/
	mcopy -i $@ kernel.bin ::/
	mcopy -i $@ text.txt ::/

# Сборка загрузчика
boot.bin: $(BOOT_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

# Сборка stage2
stage2.bin: $(STAGE2_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

# Линковка kernel
kernel.bin: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# Cборка kernel_entry
kernel_entry.o: $(KERNEL_ENTRY)
	$(ASM) -f elf32 $< -o $@

# Компиляция С-файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Запуск в QEMU
run: disk.img
	# $(QEMU) -nographic -drive format=raw,file=$<
	# $(QEMU) -display curses -serial stdio -drive format=raw,file=$<
	# $(QEMU) -display none -vnc :0 -drive format=raw,file=$<
	$(QEMU) -display vnc=:0 -vga std -drive format=raw,file=$<

# Очистка
clean:
	rm -f *.o *.bin *.img *.s

kill:
	pkill qemu-system-x86_64

ls:	disk.img
	mdir -i $< ::/

.PHONY: all run clean
