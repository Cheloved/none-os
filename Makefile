# Конфигурация
MAKE := make
BUILD_DIR := build
# QEMU := qemu-system-x86_64
QEMU := qemu-system-i386

# Исходные файлы
BOOT_DIR := bootloader/stage1
STAGE2_DIR := bootloader/stage2
KERNEL_DIR := kernel
FILES_DIR  := files


# Цели по умолчанию
all: always $(BUILD_DIR)/disk.img

# Создание образа диска
$(BUILD_DIR)/disk.img: $(BOOT_DIR)/$(BUILD_DIR)/boot.bin $(STAGE2_DIR)/$(BUILD_DIR)/stage2.bin $(KERNEL_DIR)/$(BUILD_DIR)/kernel.bin
	dd if=/dev/zero of=$@ bs=512 count=2880
	mkfs.fat -F 12 $@

	dd if=$(BOOT_DIR)/$(BUILD_DIR)/boot.bin of=$@ seek=0 conv=notrunc
	mcopy -i $@ $(KERNEL_DIR)/$(BUILD_DIR)/kernel.bin ::/
	mcopy -i $@ $(STAGE2_DIR)/$(BUILD_DIR)/stage2.bin ::/
	mcopy -i $@ $(FILES_DIR)/* ::/

# Сборка загрузчика
$(BOOT_DIR)/$(BUILD_DIR)/boot.bin:
	$(MAKE) -C $(BOOT_DIR)

# Сборка stage2
$(STAGE2_DIR)/$(BUILD_DIR)/stage2.bin:
	$(MAKE) -C $(STAGE2_DIR)

# Сборка ядра
$(KERNEL_DIR)/$(BUILD_DIR)/kernel.bin:
	$(MAKE) -C $(KERNEL_DIR)

# Запуск в QEMU
run: $(BUILD_DIR)/disk.img always
	# $(QEMU) -display vnc=:0 -vga std -fda $<
	$(QEMU) -display vnc=:0 -vga std -drive file=$<,format=raw,if=ide,index=0
	# $(QEMU) -display vnc=:0 -vga std -drive format=raw,file=$<

debug: $(BUILD_DIR)/disk.img always
	# $(QEMU) -display vnc=:0 -vga std -s -S -fda $<
	$(QEMU) -s -S -display vnc=:0 -vga std -drive file=$<,format=raw,if=ide,index=0

always:
	mkdir -p $(BUILD_DIR)

# Очистка
clean: always
	rm -f *.o *.bin *.img *.s
	rm -rf $(BUILD_DIR)
	$(MAKE) -C $(BOOT_DIR) clean
	$(MAKE) -C $(STAGE2_DIR) clean
	$(MAKE) -C $(KERNEL_DIR) clean

kill:
	pkill qemu-system-x386

ls:	$(BUILD_DIR)/disk.img always
	mdir -i $< ::/

.PHONY: all run clean kill
