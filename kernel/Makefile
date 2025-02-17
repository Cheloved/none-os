# Конфигурация
ASM := nasm
CC := gcc
LD := ld
BUILD_DIR ?= build

# Флаги
ASMFLAGS := -f elf32
CFLAGS := -O0 -nostdlib -ffreestanding -fno-pie -fno-pic \
		  -mno-red-zone -m32 -march=i386 -mtune=i386
LDFLAGS := -m elf_i386 -T kernel.ld -nostdlib --oformat binary

# Исходные файлы
KERNEL_ASM_SRCS := $(wildcard *.asm)
KERNEL_SRCS := kernel.c $(wildcard *.c)
KERNEL_OBJS := $(KERNEL_SRCS:.c=.o) $(KERNEL_ASM_SRCS:.asm=.o)

all: always $(BUILD_DIR)/kernel.bin 

$(BUILD_DIR)/kernel.bin: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.o

.PHONY: all clean always
