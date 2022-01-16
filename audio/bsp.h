/*
 * bsp.h
 */
#ifndef BSP_H_
#define BSP_H_

#include <stdint.h>
#include "stm32g0xx.h"

#define BSP_GREEN_LED_PIN  (6U)
#define BSP_BUTTON_PIN     (3U)

void delay(volatile uint32_t);
void delay_ms(volatile uint32_t);

void BSP_system_init(void);

void BSP_led_init(void);
void BSP_led_clear(void);
void BSP_led_set(void);
void BSP_led_toggle(void);

void BSP_button_init(void);
int BSP_button_read(void);

#endif
