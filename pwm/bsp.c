#include "bsp.h"
#include "stm32g0xx.h"

void BSP_system_init() {

    //SystemCoreClockUpdate();
    BSP_led_init();
    BSP_button_init();
    //SysTick_Config(SystemCoreClock / 1000);
    // setup watchdog (i/w)
    // setup delay_ms required timer
}

void delay(volatile unsigned int s) {
    for(; s>0; s--);
}

// initialize on-board led connected to C6
void BSP_led_init(void) {

    /* Enable GPIOC clock */
    RCC->IOPENR |= (1U << 2);

    /* Setup PC6 as output */
    GPIOC->MODER &= ~(3U << 2*6);
    GPIOC->MODER |= (1U << 2*6);

    /* Clear LED */
    GPIOC->BRR |= (1U << 6);

}

void BSP_led_set(void) {

    /* Turn on LED */
    GPIOC->ODR |= (1U << 6);
}

void BSP_led_clear(void) {

    /* Clear LED */
    GPIOC->BRR |= (1U << 6);
}

void BSP_led_toggle(void) {

    /* Toggle LED */
    GPIOC->ODR ^= (1U << 6);
}


// initialize on-board button connected to F2
void BSP_button_init() {

    /* Enable GPIOF clock */
    RCC->IOPENR |= (1U << 5);

    /* Setup PF2 as output */
    GPIOF->MODER &= ~(3U << 2*2);
}

// returns 1 if button is pressed
int BSP_button_read(){

    int b = ((GPIOF->IDR >> 2) & 0x0001);

    if (b) return 0;
    else return 1;
}
