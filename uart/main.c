/*
 * main.c
 *
 * author: Furkan Cayci
 */

#include <stdio.h>
#include "bsp.h"

#define PWMPERIOD 1000

void _print(char c) {
	USART2->TDR = c;
	while(!(USART2->ISR & (1 << 6)));
}

int _write(int file, char *ptr, int len)
{
    (void)file;
    for(int i=0; i<len; ++i) {
		_print(ptr[i]);
	}
    return len;
}

void print(char *s) {
	int len = 0;
	while(s[len++] != '\0');
	_write(0, s, len);
}

void USART2_IRQHandler(void) {

	char a = (char)USART2->RDR;
	_print(a); // echo back

}

void setup_UART(void) {

	/* Setup A2-3 as AF1 for UART2 TX/RX */

    /* Enable GPIOA clock */
    RCC->IOPENR |= (1U << 0);

    // set pin modes as alternate mode 1 (pins 2 and 3)

    /* Setup PA2 as alternate function */
    GPIOA->MODER &= ~(3U << 2*2);
    GPIOA->MODER |= (2U << 2*2);
    GPIOA->AFR[0] |= (1U << 4*2);
    /* Setup PA3 as alternate function */
    GPIOA->MODER &= ~(3U << 2*3);
    GPIOA->MODER |= (2U << 2*3);
    GPIOA->AFR[0] |= (1U << 4*3);

    /* Setup USART2 FIFO disabled */

    /* Enable USART2 clock */
    RCC->APBENR1 |= (1U << 17);

    USART2->CR1 = 0;
    USART2->CR1 |= (1U << 5); // enable rxneie
    USART2->CR1 |= (1U << 3); // enable transmit
    USART2->CR1 |= (1U << 2); // enable receive


    // baud rate = fCK / BAUDRATE
    // BRR = baud_rate = 16M / 115200 = 138.8 =~ 139
    USART2->BRR = 139;

    // enable usart2
    USART2->CR1 |= (1 << 0);

    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 3);

}

int main(void) {

	BSP_system_init();
	setup_UART();

	for(;;) {
		print("Hello from print!\r\n");
		printf("%s", "esto es asombroso, mi amigo!\r\n");
		delay_ms(1000);
    }

    return 0;
}
