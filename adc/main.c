/*
 * main.c
 *
 * author: Furkan Cayci
 */

#include "bsp.h"


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

    ADC1->IER |= (1U << 2); // EOCIE
    //ADC1->IER |= (1U << 3); // EOSIE

    //ADC1->CFGR1 |= (1U << 13); // Continuous mode
    ADC1->CFGR1 |= (0x0U << 3); // 12-bits, 0x2 for 8-bits
    ADC1->SMPR |= (5U << 0);// 39.5 clock cycles

    ADC1->CFGR1 |= (3U << 6); // EXTSEL choose TIM3_TRGO
    ADC1->CFGR1 |= (1U << 10); // EXTEN

    //ADC1->CFGR2 |= (1U << 30); // Clock source is PCLK/2

    ADC1->CHSELR |= (1U << 11); // Select channel 11

    ADC1->CR |= (1U << 0); // ADC EN
    while(!(ADC1->ISR & 0x01)); // wait until ADC is ready

    NVIC_SetPriority(ADC1_IRQn, 1);
    NVIC_EnableIRQ(ADC1_IRQn);

}

void TIM3_IRQHandler(void) {

	TIM3->SR &= ~(1U << 0);
}

/* Setup TIM3 TRGO */
void setup_TIM3_TRGO(void) {

    // enable TIM3 clock (bit1)
    RCC->APBENR1 |= (1U << 1);

    TIM3->CR1 = 0;
    TIM3->CR2 = (2 << 4); // 2 Update

    TIM3->CNT = 0;
    TIM3->CCR1 = 0;
    //TIM3->PSC = 90;
    //TIM3->ARR = 1000; // 6 Khz
    TIM3->PSC = 15999;
    TIM3->ARR = 1000; // 1 hz


    TIM3->CR1 |= (1 << 0); // enable TIM6

    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn,0);
}


// PB7 - Analog IN

void ADC_COMP_IRQHandler(void) {
	uint32_t val = ADC1->DR;
	double a = (3.3 * val / 4095.0);
	printf("%f\r\n", a);
	ADC1->ISR |= (1U << 3); // clear EOS
}

int main(void) {

	BSP_system_init();
	setup_UART();
	setup_ADC();
	setup_TIM3_TRGO();

    ADC1->CR |= (1U << 2); // ADC START conversion

	for(;;) {}

    return 0;
}
