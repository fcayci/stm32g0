/*
 * bsp.c
 */

#include "bsp.h"

static uint32_t tick;

void HardFault_Handler(void) {
	// Restore stack pointer
	// not really needed, but just in case
	__asm("ldr   r0, =_estack");
	__asm("mov   sp, r0");

	// log all the registers and context

	// Execute system reset
	SCB->AIRCR = (0x05FA << 16) | (1 << 2);

	// should be unreachable, but just in case
	__asm("dsb");
	for(;;);
}

void WWDG_IRQHandler(void) {
	for(;;);
}

void NMI_Handler(void) {
	for(;;);
}


void SysTick_Handler(void) {
	if (tick > 0) {
		--tick;
	}
}

void USART2_IRQHandler(void) {

	char a = (char)USART2->RDR;
	_print(a);                    // echo back
}

int _write(int file, char *ptr, int len)
{
    (void)file;
    for(int i=0; i<len; ++i) {
		_print(ptr[i]);
	}
    return len;
}

void print(char *c) {
	int len = 0;
	while(c[len++] != '\0');
	_write(0, c, len);
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
    USART2->BRR = (uint32_t)(SystemCoreClock / 115200);

    // enable usart2
    USART2->CR1 |= (1 << 0);

    // Enable IRQ from NVIC with lowest priority
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 3);
}


void delay(volatile uint32_t s) {
    for(; s>0; s--);
}


// blocking operation
void delay_ms(volatile uint32_t s) {
	tick = s;
	while(tick);
}


void BSP_system_init(void) {
	__disable_irq();
	SysTick_Config(SystemCoreClock / 1000);
	BSP_led_init();
	BSP_button_init();
	__enable_irq();
}


void BSP_led_init(void) {

    /* Enable GPIOC clock */
    RCC->IOPENR |= (1U << 2);

    /* Setup PC6 as output */
    GPIOC->MODER &= ~(3U << 2*BSP_GREEN_LED_PIN);
    GPIOC->MODER |= (1U << 2*BSP_GREEN_LED_PIN);

    /* Turn off LED */
    GPIOC->BRR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_led_set(void) {

    /* Turn on LED */
    GPIOC->ODR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_led_clear(void) {

    /* Turn off LED */
    GPIOC->BRR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_led_toggle(void) {

    /* Turn off LED */
    GPIOC->ODR ^= (1U << BSP_GREEN_LED_PIN);
}


void BSP_button_init(void) {

    /* Enable GPIOB clock */
    RCC->IOPENR |= (1U << 1);

    /* Setup PB3 as input */
    GPIOB->MODER &= ~(3U << 2*BSP_BUTTON_PIN);
}

int BSP_button_read(void) {

    /* Button read */
    int b = (GPIOB->IDR >> BSP_BUTTON_PIN) & 0x01;
    /* button is active low */
    if (b) return 0;
    else return 1;
}

