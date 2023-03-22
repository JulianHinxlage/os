CXX = g++
CC = g++
ASM = nasm
ASM_FLAGS = -f elf
CFLAGS = -ffreestanding -nostdlib -fno-pic -m32 -O0 -s -std=c++17 -I kernel/
LD = gcc
LDFLAGS = $(CFLAGS)

BIN_DIR = ../build

BOOT_STAGE1 = $(BIN_DIR)/bootloader/stage1.bin
BOOT_PATCHER = $(BIN_DIR)/bootloader/boot_patcher.out

KERNEL = $(BIN_DIR)/kernel/kernel.bin
IMG = $(BIN_DIR)/os.img


SRC_CPP= \
	$(wildcard kernel/*.cpp) \
	$(wildcard kernel/*/*.cpp) \
	$(wildcard kernel/*/*/*.cpp) \
	$(wildcard kernel/*/*/*/*.cpp) \

SRC_HPP= \
	$(wildcard kernel/*.hpp) \
	$(wildcard kernel/*/*.hpp) \
	$(wildcard kernel/*/*/*.hpp) \
	$(wildcard kernel/*/*/*/*.hpp) \

SRC_C= \
	$(wildcard kernel/*.c) \
	$(wildcard kernel/*/*.c) \
	$(wildcard kernel/*/*/*.c) \
	$(wildcard kernel/*/*/*/*.c) \
	
SRC_H= \
	$(wildcard kernel/*.h) \
	$(wildcard kernel/*/*.h) \
	$(wildcard kernel/*/*/*.h) \
	$(wildcard kernel/*/*/*/*.h) \

SRC_ASM= \
	$(wildcard kernel/*.asm) \
	$(wildcard kernel/*/*.asm) \
	$(wildcard kernel/*/*/*.asm) \
	$(wildcard kernel/*/*/*/*.asm) \
	
OBJS_CPP=$(patsubst %.cpp, $(BIN_DIR)/%.o, $(SRC_CPP))
OBJS_C=$(patsubst %.c, $(BIN_DIR)/%.o, $(SRC_C))
OBJS_ASM=$(patsubst %.asm, $(BIN_DIR)/%.asm.o, $(SRC_ASM))
OBJS = $(OBJS_C) $(OBJS_ASM) $(OBJS_CPP)

always:
	mkdir -p ../build
	mkdir -p ../build/kernel
	mkdir -p ../build/kernel/arch
	mkdir -p ../build/kernel/arch/x86
	mkdir -p ../build/kernel/driver
	mkdir -p ../build/kernel/io
	mkdir -p ../build/kernel/hal
	mkdir -p ../build/kernel/memory
	mkdir -p ../build/kernel/util
	mkdir -p ../build/bootloader

# boot image
os_fat12: always $(KERNEL) $(BOOT_STAGE1) $(BOOT_PATCHER)
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	mkfs.fat -F 12 -n "PROJECT OS" $(IMG)
	mcopy -i $(IMG) $(KERNEL) "::kernel.bin"
	$(BOOT_PATCHER) $(IMG) $(BOOT_STAGE1) $(KERNEL)

os_fat16: always $(KERNEL) $(BOOT_STAGE1) $(BOOT_PATCHER)
	dd if=/dev/zero of=$(IMG) bs=512 count=32768
	mkfs.fat -F 16 -n "PROJECT OS" $(IMG)
	mcopy -i $(IMG) $(KERNEL) "::kernel.bin"
	$(BOOT_PATCHER) $(IMG) $(BOOT_STAGE1) $(KERNEL)

os_fat32: always $(KERNEL) $(BOOT_STAGE1) $(BOOT_PATCHER)
	dd if=/dev/zero of=$(IMG) bs=512 count=131072
	mkfs.fat -F 32 -n "PROJECT OS" $(IMG)
	mcopy -i $(IMG) $(KERNEL) "::kernel.bin"
	$(BOOT_PATCHER) $(IMG) $(BOOT_STAGE1) $(KERNEL)


# boot loader
$(BOOT_PATCHER): bootloader/boot_patcher.cpp
	$(CXX) $< -o $@

$(BOOT_STAGE1): bootloader/stage1.asm
	$(ASM) -f bin $< -o $@


# Kernel
$(KERNEL): $(OBJS) $(SRC_H) $(SRC_HPP)
	$(LD) $(LDFLAGS) -T kernel/linker.lnk $(OBJS) -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.asm.o : %.asm
	$(ASM) $(ASM_FLAGS) $< -o $@
