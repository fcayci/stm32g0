#ifndef BSP_H_
#define BSP_H_

#include "stm32g0xx.h"

/* Common API function for Nucleo STM32G031 board */

void BSP_system_init();

void delay(volatile unsigned int);

// LED related functions
void BSP_led_init();
void BSP_led_set();
void BSP_led_clear();
void BSP_led_toggle();


// Button related functions
void BSP_button_init();
int BSP_button_read();


#endif
