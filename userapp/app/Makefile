PREFIX = arm-none-eabi-
CC     = $(PREFIX)gcc

CPU     = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS  = $(CPU) -Og -g -nostdlib -fno-builtin -Wall
LDFLAGS = -T user.ld -Wl,--emit-relocs

hello.elf: entry.S main.c user.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ entry.S main.c

hello.flat: hello.elf
	python ../tools/elf2flat.py hello.elf hello.flat

clean:
	rm -f *.elf *.flat