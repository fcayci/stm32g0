/*
 * main.c
 *
 * author: Furkan Cayci
 */

#include "bsp.h"

#define PWMPERIOD 1000

void TIM3_IRQHandler(void) {
	static int duty = 0;
	static int mode = 0;

	if (TIM3->SR & 0x0001) // UIE
	{
    	BSP_led_toggle();

    	switch (mode) {
    		default:
			case 0:
				duty += 10;
				if (duty > PWMPERIOD) {
					mode = 1;
					duty -= 10;
				}
				break;
			case 1:
				duty -= 10;
				if (duty < 0) {
					mode = 0;
					duty += 10;
				}
				break;
    	}

    	TIM3->CCR1 = (uint32_t)duty;
		TIM3->SR &= ~(1U << 0);
	}
}

void setup_PWM(void) {

	/* Setup B4 as AF1 for TIM3 Ch1 */

    /* Enable GPIOB clock */
    RCC->IOPENR |= (1U << 1);

    /* Setup PB4 as alternate function */
    GPIOB->MODER &= ~(3U << 2*4);
    GPIOB->MODER |= (2U << 2*4);

    GPIOB->AFR[0] |= (1U << 4*4);

    /* Setup TIM3 Ch1 */

    // enable TIM3 clock (bit1)
    RCC->APBENR1 |= (1 <<1);

    TIM3->CR1 = 0;
    TIM3->DIER |= (1 << 0); // UIE
    //TIM3->DIER |= (1 << 1); // CC1IE

    TIM3->CCMR1 |= (0x6 << 4); // OC1M PWM mode 1
    TIM3->CCMR1 |= (1 << 3); // OC1PE preload enable
    TIM3->CCER  |= (1 << 0); // Capture compare ch1 enable

    TIM3->CNT = 0;
    TIM3->CCR1 = 0;
    TIM3->PSC = 15;
    TIM3->ARR = PWMPERIOD; // 1ms

    TIM3->CR1 |= (1 << 0); // enable TIM3

    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn,0);
}

int main(void) {

	BSP_system_init();
	setup_PWM();

	for(;;) {
    }

    return 0;
}
