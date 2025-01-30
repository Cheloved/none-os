# Конфигурация
ASM := nasm
BUILD_DIR ?= build

# Флаги
ASMFLAGS := -f bin

# Исходные файлы
BOOT_SRC := boot.asm

all: always $(BUILD_DIR)/boot.bin 

$(BUILD_DIR)/boot.bin: $(BOOT_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean always
