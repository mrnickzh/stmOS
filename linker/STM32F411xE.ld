MEMORY {
    FLASH   : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM    : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS {
    .vector_table : {
        *(.vector_table)
    } > FLASH

    .text : {
        *(.text)
        . = ALIGN(4);
    } > FLASH
	
	.rodata : {
		*(.rodata)
		*(.rodata*)
	} > FLASH

    .stack (NOLOAD) : {
        . = . + 0x800;
        __stack_end = .;
    } > SRAM

    .data : {
        __data_begin = .;
        *(.data)
        . = ALIGN(4);
        __data_end = .;
    } > SRAM AT > FLASH

    __data_load = LOADADDR(.data);

    .bss (NOLOAD) : {
        __bss_begin = .;
        *(.bss)
        . = ALIGN(4);
        __bss_end = .;
    } > SRAM
	
	.stack_pool (NOLOAD) : ALIGN(8) {
        _stack_pool_start = .;
        KEEP(*(.stack_pool))
        _stack_pool_end = .;
    } > SRAM
	
	.page_pool (NOLOAD) : ALIGN(8) {
        _page_pool_start = .;
        KEEP(*(.page_pool))
        _page_pool_end = .;
    } > SRAM
}