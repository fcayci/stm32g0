/*
 * main.c
 *
 * author: Furkan Cayci
 */

#include "stm32g0xx.h"

#define LEDDELAY    1600000

#define BADADDRESS 0x60000000
volatile uint32_t *badaddr = (uint32_t *)BADADDRESS;

void delay(volatile uint32_t);

void HardFault_Handler(void) {
	// Restore stack pointer
	// not really needed, but just in case
	__asm("ldr   r0, =_estack");
	__asm("mov   sp, r0");

	// log all the registers and context

	// Execute system reset
	SCB->AIRCR = (0x05FA << 16) | (1 << 2);

	// should be unreachable, but just in case
	__asm("dsb");
	for(;;);
}

int main(void) {

    /* Enable GPIOC clock */
    RCC->IOPENR |= (1U << 2);

    /* Setup PC6 as output */
    GPIOC->MODER &= ~(3U << 2*6);
    GPIOC->MODER |= (1U << 2*6);

    /* Turn on LED */
    GPIOC->ODR |= (1U << 6);

    for(int i=0; i < 6; ++i) {
        delay(LEDDELAY);
        GPIOC->ODR ^= (1U << 6);
    }

    *badaddr = 1; // fault

    for(;;); // trap

    return 0;
}

void delay(volatile uint32_t s) {
    for(; s>0; s--);
}
