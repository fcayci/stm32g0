/*
 * standalone.c
 *
 * author: Furkan Cayci
 *
 * description: Added the necessary stuff for blinking an LED on the G031K8
 * Nucleo board. No external libraries are used. Should not be used for any
 * projects.
 *
 * Any operations that require libraries will probably fail.
 */

#ifndef uint32_t
#define uint32_t unsigned int
#endif

/* Define peripheral addresses from RM0444 page 57, Tables 3-4 */
#define RCC_BASE         (0x40021000)          // RCC base address
#define RCC_IOPENR_ADDR  (RCC_BASE + (0x34))   // RCC IOPENR register offset

#define GPIOC_BASE       (0x50000800)          // GPIOC base address
#define GPIOC_MODER_ADDR (GPIOC_BASE + (0x00)) // GPIOC MODER register offset
#define GPIOC_ODR_ADDR   (GPIOC_BASE + (0x14)) // GPIOC ODR register offset

/* Define pointers to the addresses */
volatile uint32_t* RCC_IOPENR  = (uint32_t *)(RCC_IOPENR_ADDR);
volatile uint32_t* GPIOC_ODR   = (uint32_t *)(GPIOC_ODR_ADDR);
volatile uint32_t* GPIOC_MODER = (uint32_t *)(GPIOC_MODER_ADDR);

int main(void);
void Default_Handler(void);
void Reset_Handler(void);
void _init_data(void);

typedef void (* pFunc)(void);
extern unsigned long _estack;

__attribute__ ((section(".vectors")))
pFunc vector_table[] = {
    (pFunc)(&_estack), /* 0x000 Stack Pointer */
    Reset_Handler,     /* 0x004 Reset         */
    Default_Handler,   /* 0x008 NMI           */
    Default_Handler    /* 0x00C HardFault     */
    /* rest of the interrupts should go here  */
};

/* initialize data and bss sections */
void _init_data(void) {
    /* these are symbols from linker script */
    extern unsigned long __etext, __data_start__, __data_end__, __bss_start__, __bss_end__;
    unsigned long *src = &__etext;
    unsigned long *dst = &__data_start__;

    /* ROM has data at end of text. copy it */
    while (dst < &__data_end__)
        *dst++ = *src++;

    /* zero bss */
    for (dst = &__bss_start__; dst< &__bss_end__; dst++)
        *dst = 0;
}

/* Entry point, calls main after prologue */
void Reset_Handler(void) {
    _init_data();
    main();
    for(;;);
}

/* All faults will end up here, since we did not define the rest of the vector
 * table, the other exceptions/interrupts will most likely crash the program.
 */
void Default_Handler(void) { for(;;); }

int main(void) {

    /* Enable GPIOC clock */
    *RCC_IOPENR |= (1U << 2);

    /* Setup PC6 for led */
    *GPIOC_MODER &= ~(uint32_t)(3U << 2*6);
    *GPIOC_MODER |= (1U << 2*6);
    *GPIOC_ODR |= (1U << 6);

    /* Blink led */
    while(1) {
        for(volatile uint32_t s=100000; s>0; s--);
        *GPIOC_ODR ^= (1U << 6);
    }

    return 0;
}
