# ----- Toolchain -----
BASE     = arm-none-eabi-
AS       = $(BASE)gcc
CC       = $(BASE)gcc
LD       = $(BASE)gcc
OBJCOPY  = $(BASE)objcopy
GDB      = $(BASE)gdb
ST-FLASH = st-flash
ST-UTIL  = st-util

# ----- Sources ------
BUILD_DIR = build
C_SRCS   = $(wildcard src/*.c)
ASM_SRCS = $(wildcard startup/*.S) $(wildcard src/*.S)
OBJS = $(patsubst %.S,$(BUILD_DIR)/%.o,$(notdir $(ASM_SRCS))) \
	   $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SRCS)))

vpath %.c src
vpath %.S src startup

# ----- Flags -----
CPU = -mcpu=cortex-m4 -mthumb

ASFLAGS  =
CFLAGS   = $(CPU) -Wall -Wextra -Wpedantic -std=c11 -g -O2 -DOS_DEBUG
CPPFLAGS = -MMD -MP
LDFLAGS  = $(CPU) -Tlinker/STM32F411xE.ld -nostartfiles
LDLIBS   =

# ----- Target -----
TARGET = program

# ----- Outputs -----
ELF = $(TARGET).elf
BIN = $(TARGET).bin

# ----- Default target -----
all: $(ELF) $(BIN)

# ----- Link -----
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# ----- Compile ASM -----
$(BUILD_DIR)/%.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

# ----- Compile C -----
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----- Binary format -----
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $<  $@

# ---- Clean ----
.PHONY: clean
clean:
	C:/msys64/usr/bin/rm -rf build/*.o
	C:/msys64/usr/bin/rm -rf build/*.d
	C:/msys64/usr/bin/rm -rf *.bin
	C:/msys64/usr/bin/rm -rf *.elf