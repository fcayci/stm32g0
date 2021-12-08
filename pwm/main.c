/*
 * main.c
 */

#include "bsp.h"

void TIM2_IRQHandler(void) {
    // update duty (CCR2)

    // Clear update status register
    TIM2->SR &= ~(1U << 0); 
}

void init_pwm2() {

    // enable TIM2 module clock
    RCC->APBENR1 |= RCC_APBENR1_TIM2EN;

    // zero out the control register just in case
    TIM2->CR1 = 0;

    TIM2->CCMR1 |= (6U << 12);
    TIM2->CCMR1 |= TIM_CCMR1_OC2PE;

    // Capture compare ch2 enable
    TIM2->CCER |= TIM_CCER_CC2E;

    // zero out counter
    TIM2->CNT = 0;
    // 1 ms interrupt
    TIM2->PSC = 3;
    TIM2->ARR = 4000;

    // zero out duty
    TIM2->CCR2 = 0;

    // Update interrupt enable
    TIM2->DIER |= (1 << 0);

    // TIM1 Enable
    TIM2->CR1 |= (1 << 0);

    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}

int main(void) {

    BSP_system_init();
    init_pwm2();

    // trap
    for(;;);

    return 0;
}
