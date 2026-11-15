#ifndef CORTEX_M4_H
#define CORTEX_M4_H

#define NULL (void*)(0)

#include <stdint.h>

// NVIC peripheral registers structure
struct NVIC_t
{
    volatile uint32_t ISER[8];          // Interrupt set-enable register.
    volatile uint32_t RESERVED0[24];    // Reserved.
    volatile uint32_t ICER[8];          // Interrupt clear-enable register.
    volatile uint32_t RESERVED1[24];    // Reserved.
    volatile uint32_t ISPR[8];          // Interrupt set-pending register.
    volatile uint32_t RESERVED2[24];    // Reserved.
    volatile uint32_t ICPR[8];          // Interrupt clear-pending register.
    volatile uint32_t RESERVED3[24];    // Reserved.
    volatile uint32_t IABR[8];          // Interrupt active bit register.
    volatile uint32_t RESERVED4[56];    // Reserved.
    volatile uint8_t  IP[240];          // Interrupt priority register.
    volatile uint32_t RESERVED5[644];   // Reserved.
    volatile uint32_t STIR;             // Software trigger interrupt register.
};

// SysTick peripheral registers structure
struct STK_t
{
    volatile uint32_t CTRL;     // SysTick control and status register. Address offset: 0x00
    volatile uint32_t LOAD;     // SysTick reload value register.       Address offset: 0x04
    volatile uint32_t VAL;      // SysTick current value register.      Address offset: 0x08
    volatile uint32_t CALIB;    // SysTick calibration value register.  Address offset: 0x0C
};

// Peripheral base addresses
#define NVIC_BASE   0xE000E100
#define STK_BASE    0xE000E010

// Peripheral declarations
#define NVIC    ((struct NVIC_t*)NVIC_BASE)
#define STK     ((struct STK_t*)STK_BASE)

// Bit definitions for STK_CTRL register
#define STK_CTRL_COUNTFLAG      (1UL << 16)
#define STK_CTRL_CLKSOURCE      (1UL << 2)
#define STK_CTRL_TICKINT        (1UL << 1)
#define STK_CTRL_ENABLE         (1UL << 0)

// Bit definitions for STK_LOAD register
#define STK_LOAD_RELOAD_MASK    0x00FFFFFF

// Bit definitions for STK_VAL register
#define STK_LOAD_VAL_MASK       0x00FFFFFF

// SHPRs
#define SHPR2   0xE000ED1C
#define SHPR3   0xE000ED20

// PENDSVSET
#define PENDSVSET 0xE000ED04

// Enable interrupt in the NVIC.
static inline void irq_enable(uint32_t IRQn)
{
    NVIC->ISER[IRQn / 32] = (1UL << (IRQn % 32));
}

static inline uint32_t irq_save(void) {
    uint32_t primask;
    __asm volatile("mrs %0, primask" : "=r"(primask) :: "memory");
    __asm volatile("cpsid i" ::: "memory");
    return primask;
}

static inline void irq_restore(uint32_t primask) {
    __asm volatile("msr primask, %0" :: "r"(primask) : "memory");
}

#endif // CORTEX_M4_H
