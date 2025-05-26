# Makefile per SeaOS

AS=nasm
CC=gcc
LD=ld
CFLAGS=-m32 -ffreestanding -c
LDFLAGS=-m elf_i386 -T src/linker.ld

SRC_DIR=src
ISO_DIR=$(SRC_DIR)/iso
BUILD_DIR=build

KERNEL_OBJS=$(BUILD_DIR)/multiboot_header.o $(BUILD_DIR)/kernel_entry.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/shell.o
KERNEL_BIN=$(BUILD_DIR)/kernel.bin
ISO_IMG=SeaOS.iso

all: $(KERNEL_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/multiboot_header.o: $(SRC_DIR)/multiboot_header.asm | $(BUILD_DIR)
	$(AS) -f elf $< -o $@

$(BUILD_DIR)/kernel_entry.o: $(SRC_DIR)/kernel_entry.asm | $(BUILD_DIR)
	$(AS) -f elf $< -o $@

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/shell.o: $(SRC_DIR)/shell.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

iso: all
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	grub-mkrescue -o $(ISO_IMG) $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ISO_IMG)

.PHONY: all iso clean
