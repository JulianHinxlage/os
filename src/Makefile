CXX = g++
CC = g++
ASM = nasm
ASM_FLAGS = -f elf
CFLAGS = -ffreestanding -nostdlib -fno-pic -m32 -O0 -s -std=c++98
LD = i686-linux-gnu-ld
LDFLAGS =

BIN_DIR = ../build

BOOT_STAGE1 = $(BIN_DIR)/bootloader/stage1.bin
BOOT_PATCHER = $(BIN_DIR)/bootloader/stage1_patcher.out

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

# boot image
os: $(KERNAL) $(BOOT_STAGE1) $(BOOT_PATCHER)
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	mkfs.fat -F 12 -n "OS" $(IMG)
	$(BOOT_PATCHER) $(BOOT_STAGE1) $(KERNAL)
	dd if=$(BOOT_STAGE1) of=$(IMG) conv=notrunc
	mcopy -i $(IMG) $(KERNAL) "::kernal.bin"

# boot loader
$(BOOT_PATCHER): bootloader/stage1_patcher.cpp
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
