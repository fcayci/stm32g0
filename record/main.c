/*
 * main.c
 *
 * author: Furkan Cayci
 *
 * PB7 - Analog IN
 * PB4 - PWM OUT
 */

#include "bsp.h"

uint8_t data;

void TIM2_IRQHandler(void) {

	TIM2->CCR1 = (uint32_t)data;
	TIM2->SR &= ~(1U << 0);
}

void ADC_COMP_IRQHandler(void) {
	data = (uint8_t)ADC1->DR;

	ADC1->ISR |= (1U << 3); // clear EOS
}

/* Setup TIM2 PWM
 * Currently it runs at 8 khz
 *
 * Connect B4 to speaker
 */
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
    TIM3->ARR = 125; // 8 Khz

    TIM3->CR1 |= (1 << 0); // enable TIM3

    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn,0);
}

/*
 *  Setup ADC.
 *
 *  Conenct B7 to Mic
 */
void setup_ADC() {

    /* Enable GPIOB clock */
    RCC->IOPENR |= (1U << 1);

    /* Setup PB7 as analog input */
    GPIOB->MODER &= ~(3U << 2*7);
    GPIOB->MODER |= (3U << 2*7);

    /* Enable ADC clock */
    RCC->APBENR2 |= (1U << 20);

    ADC1->CR |= (1U << 28); // ADC Voltage regulator enable
    for(volatile int i=0; i<1000000; ++i); // wait a long time for regulator

    ADC1->CR |= (1U << 31); // Calibrate ADC
    while(!(ADC1->ISR & (1U << 11))); // wait until calibration complete

    //ADC1->IER |= (1U << 2); // EOCIE
    ADC1->IER |= (1U << 3); // EOSIE

    //ADC1->CFGR1 |= (1U << 13); // Continuous mode
    ADC1->CFGR1 |= (0x2U << 3); // 0x0 for 12-bits, 0x2 for 8-bits
    ADC1->SMPR |= (5U << 0);// 39.5 clock cycles

    ADC1->CFGR1 |= (2U << 6); // EXTSEL choose TIM2_TRGO
    ADC1->CFGR1 |= (1U << 10); // EXTEN

    //ADC1->CFGR2 |= (1U << 30); // Clock source is PCLK/2

    ADC1->CHSELR |= (1U << 11); // Select channel 11

    ADC1->CR |= (1U << 0); // ADC EN
    while(!(ADC1->ISR & 0x01)); // wait until ADC is ready

    NVIC_SetPriority(ADC1_IRQn, 1);
    NVIC_EnableIRQ(ADC1_IRQn);

}

/* Setup TIM2 TRGO
 * Currently it runs at 8 khz
 */
void setup_TIM2_TRGO(void) {

    // enable TIM2 clock (bit0)
    RCC->APBENR1 |= (1U << 0);

    TIM2->CR1 = 0;
    TIM2->CR2 = (2 << 4); // 2 Update

    TIM2->CNT = 0;
    TIM2->CCR1 = 0; // This is the value we will update
    TIM2->PSC = 15;
    TIM2->ARR = 125; // 8 Khz

    TIM2->CR1 |= (1 << 0); // enable TIM2

    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn,0);
}


int main(void) {

	BSP_system_init();
	setup_UART();

	/* Setup ADC */
	setup_ADC();
	setup_TIM2_TRGO();

	/* Setup PWM */
	setup_PWM();

    ADC1->CR |= (1U << 2); // ADC START conversion

	for(;;) {}

    return 0;
}
