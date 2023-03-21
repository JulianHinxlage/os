CXX = g++
CC = g++
ASM = nasm
ASM_FLAGS = -f elf
CFLAGS = -ffreestanding -nostdlib -fno-pic -m32 -O0 -s -std=c++17 -I kernal/
LD = gcc
LDFLAGS = $(CFLAGS)

BIN_DIR = ../build

BOOT_STAGE1 = $(BIN_DIR)/bootloader/stage1.bin
BOOT_PATCHER = $(BIN_DIR)/bootloader/boot_patcher.out

KERNAL = $(BIN_DIR)/kernal/kernal.bin
IMG = $(BIN_DIR)/os.img


SRC_CPP= \
	$(wildcard kernal/*.cpp) \
	$(wildcard kernal/*/*.cpp) \
	$(wildcard kernal/*/*/*.cpp) \
	$(wildcard kernal/*/*/*/*.cpp) \

SRC_HPP= \
	$(wildcard kernal/*.hpp) \
	$(wildcard kernal/*/*.hpp) \
	$(wildcard kernal/*/*/*.hpp) \
	$(wildcard kernal/*/*/*/*.hpp) \

SRC_C= \
	$(wildcard kernal/*.c) \
	$(wildcard kernal/*/*.c) \
	$(wildcard kernal/*/*/*.c) \
	$(wildcard kernal/*/*/*/*.c) \
	
SRC_H= \
	$(wildcard kernal/*.h) \
	$(wildcard kernal/*/*.h) \
	$(wildcard kernal/*/*/*.h) \
	$(wildcard kernal/*/*/*/*.h) \

SRC_ASM= \
	$(wildcard kernal/*.asm) \
	$(wildcard kernal/*/*.asm) \
	$(wildcard kernal/*/*/*.asm) \
	$(wildcard kernal/*/*/*/*.asm) \
	
OBJS_CPP=$(patsubst %.cpp, $(BIN_DIR)/%.o, $(SRC_CPP))
OBJS_C=$(patsubst %.c, $(BIN_DIR)/%.o, $(SRC_C))
OBJS_ASM=$(patsubst %.asm, $(BIN_DIR)/%.asm.o, $(SRC_ASM))
OBJS = $(OBJS_C) $(OBJS_ASM) $(OBJS_CPP)

always:
	mkdir -p ../build
	mkdir -p ../build/kernal
	mkdir -p ../build/kernal/arch
	mkdir -p ../build/kernal/arch/x86
	mkdir -p ../build/kernal/driver
	mkdir -p ../build/kernal/io
	mkdir -p ../build/kernal/hal
	mkdir -p ../build/kernal/memory
	mkdir -p ../build/kernal/util
	mkdir -p ../build/bootloader

# boot image
os_fat12: always $(KERNAL) $(BOOT_STAGE1) $(BOOT_PATCHER)
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	mkfs.fat -F 12 -n "PROJECT OS" $(IMG)
	mcopy -i $(IMG) $(KERNAL) "::kernal.bin"
	$(BOOT_PATCHER) $(IMG) $(BOOT_STAGE1) $(KERNAL)

os_fat16: always $(KERNAL) $(BOOT_STAGE1) $(BOOT_PATCHER)
	dd if=/dev/zero of=$(IMG) bs=512 count=32768
	mkfs.fat -F 16 -n "PROJECT OS" $(IMG)
	mcopy -i $(IMG) $(KERNAL) "::kernal.bin"
	$(BOOT_PATCHER) $(IMG) $(BOOT_STAGE1) $(KERNAL)

os_fat32: always $(KERNAL) $(BOOT_STAGE1) $(BOOT_PATCHER)
	dd if=/dev/zero of=$(IMG) bs=512 count=131072
	mkfs.fat -F 32 -n "PROJECT OS" $(IMG)
	mcopy -i $(IMG) $(KERNAL) "::kernal.bin"
	$(BOOT_PATCHER) $(IMG) $(BOOT_STAGE1) $(KERNAL)


# boot loader
$(BOOT_PATCHER): bootloader/boot_patcher.cpp
	$(CXX) $< -o $@

$(BOOT_STAGE1): bootloader/stage1.asm
	$(ASM) -f bin $< -o $@


# Kernal
$(KERNAL): $(OBJS) $(SRC_H) $(SRC_HPP)
	$(LD) $(LDFLAGS) -T kernal/linker.lnk $(OBJS) -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.asm.o : %.asm
	$(ASM) $(ASM_FLAGS) $< -o $@
