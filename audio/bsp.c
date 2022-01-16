/*
 * bsp.c
 */

#include "bsp.h"

static uint32_t tick;

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

void WWDG_IRQHandler(void) {
	for(;;);
}

void NMI_Handler(void) {
	for(;;);
}

void SysTick_Handler(void) {
	if (tick > 0) {
		--tick;
	}
}


void delay(volatile uint32_t s) {
    for(; s>0; s--);
}

void delay_ms(volatile uint32_t s) {
	tick = s;
	while(tick);
}


void BSP_system_init(void) {
	__disable_irq();
	SysTick_Config(SystemCoreClock / 1000);
	BSP_led_init();
	BSP_button_init();
	__enable_irq();
}


void BSP_led_init(void) {

    /* Enable GPIOC clock */
    RCC->IOPENR |= (1U << 2);

    /* Setup PC6 as output */
    GPIOC->MODER &= ~(3U << 2*BSP_GREEN_LED_PIN);
    GPIOC->MODER |= (1U << 2*BSP_GREEN_LED_PIN);

    /* Turn off LED */
    GPIOC->BRR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_led_set(void) {

    /* Turn on LED */
    GPIOC->ODR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_led_clear(void) {

    /* Turn off LED */
    GPIOC->BRR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_led_toggle(void) {

    /* Turn off LED */
    GPIOC->ODR ^= (1U << BSP_GREEN_LED_PIN);
}


void BSP_button_init(void) {

    /* Enable GPIOB clock */
    RCC->IOPENR |= (1U << 1);

    /* Setup PF2 as input */
    GPIOB->MODER &= ~(3U << 2*BSP_BUTTON_PIN);
}

int BSP_button_read(void) {

    /* Button read */
    int b = (GPIOB->IDR >> BSP_BUTTON_PIN) & 0x01;
    /* button is active low */
    if (b) return 0;
    else return 1;
}

