ENTRY(_start)
SECTIONS {
    . = 0x00000000;
    
    .text : {
        *(.text.entry)      /* _start entry point */
        *(.text*) *(.rodata*)
        . = ALIGN(4);
    }
    
    _sdata = .;
    .data : {
        *(.data*)
        . = ALIGN(4);
    }
    _edata = .;
    
    _sbss = .;
    .bss : {
        *(.bss*) *(COMMON)
        . = ALIGN(4);
    }
    _ebss = .;
}