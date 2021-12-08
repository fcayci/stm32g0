/*
 * main.c
 */

#include "bsp.h"

void TIM2_IRQHandler(void) {
    // update duty (CCR2)

    // Clear update status register
    TIM2->SR &= ~(1U << 0); 
}


/**
 * Setup PWM output for
 * TIM2 CH2 on PB3
 */
void init_pwm2() {

    // Setup GPIO
    //

    // Enable GPIOB clock
    RCC->IOPENR |= (1U << 1);
    // Enable TIM2 clock
    RCC->APBENR1 |= RCC_APBENR1_TIM2EN;

    // Set alternate function to 2
    // 3 comes from PB3
    GPIOB->AFR[0] |= (2U << 4*3);
    // Select AF from Moder
    GPIOB->MODER &= ~(3U << 2*3);
    GPIOB->MODER |= (2U << 2*3);

    // zero out the control register just in case
    TIM2->CR1 = 0;

    // Select PWM Mode 1
    TIM2->CCMR1 |= (6U << 12);
    // Preload Enable
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
    TIM2->CR1 |= TIM_CR1_CEN;

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
