# ----- Recursive Wildcard -----
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

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
C_SRCS   = $(call rwildcard,src,*.c)
ASM_SRCS = $(call rwildcard,startup,*.S) $(call rwildcard,src,*.S)
OBJS = $(patsubst %.S,$(BUILD_DIR)/%.o,$(notdir $(ASM_SRCS))) \
	   $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SRCS)))

SRC_DIRS = $(sort $(dir $(C_SRCS) $(ASM_SRCS)))
vpath %.c $(SRC_DIRS)
vpath %.S $(SRC_DIRS)

# ----- Flags -----
CPU = -mcpu=cortex-m4 -mthumb

ASFLAGS  =
CFLAGS   = $(CPU) -Wall -Wextra -Wpedantic -std=c11 -g -O2 -DOS_DEBUG
CPPFLAGS = -MMD -MP -Iinclude
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
	
# ----- Create build dir -----
$(BUILD_DIR):
	C:/msys64/usr/bin/mkdir -p $@

# ---- Clean ----
.PHONY: clean
clean:
	C:/msys64/usr/bin/rm -rf build/*.o
	C:/msys64/usr/bin/rm -rf build/*.d
	C:/msys64/usr/bin/rm -rf *.bin
	C:/msys64/usr/bin/rm -rf *.elf