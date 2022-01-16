/*
 * bsp.h
 */
#ifndef BSP_H_
#define BSP_H_

#include <stdint.h>
#include <stdio.h>
#include "stm32g0xx.h"

#define BSP_GREEN_LED_PIN  (6U)
#define BSP_BUTTON_PIN     (3U)

void print(char *);

void setup_UART(void);

void delay(volatile uint32_t);
void delay_ms(volatile uint32_t);

void BSP_system_init(void);

void BSP_led_init(void);
void BSP_led_clear(void);
void BSP_led_set(void);
void BSP_led_toggle(void);

void BSP_button_init(void);
int BSP_button_read(void);


__STATIC_INLINE void _print(char c) {
	USART2->TDR = c;
	while(!(USART2->ISR & (1 << 6)));
}



#endif
