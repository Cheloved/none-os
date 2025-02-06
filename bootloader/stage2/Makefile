# Конфигурация
ASM := nasm
CC := gcc
LD := ld
BUILD_DIR ?= build

# Флаги
ASMFLAGS := -f elf32
CFLAGS := -O0 -nostdlib -ffreestanding -fno-pie -fno-pic \
		  -mno-red-zone -m16 -march=i386 -mtune=i386
LDFLAGS := -m elf_i386 -T stage2.ld -nostdlib --oformat binary

# Исходные файлы
STAGE2_ENTRY := stage2_entry.asm
STAGE2_ASM_SRCS := $(wildcard *.asm)
STAGE2_SRCS := stage2.c $(wildcard *.c)
STAGE2_OBJS := $(STAGE2_ENTRY:.asm=.o) $(STAGE2_SRCS:.c=.o) $(STAGE2_ASM_SRCS:.asm=.o) 

all: always $(BUILD_DIR)/stage2.bin 

$(BUILD_DIR)/stage2.bin: $(STAGE2_OBJS)
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
