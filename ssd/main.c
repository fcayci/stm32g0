/*
 * main.c
 *
 *   Connected SSD pins:
 *   A0   - A
 *   A1   - B
 *   A4   - C
 *   A5   - D
 *   A12  - E
 *   A11  - F
 *   A6   - G
 *   A7   - CAT
 *
 * author: Furkan Cayci
 */

#include "bsp.h"

#define global volatile

#define MAX_DISP          (99L)
#define MIN_DISP          (-9L)

#define SSD_SELECT_PIN    (7)
#define SSD_SELECT        (1U << SSD_SELECT_PIN)

#define __SELECT_FIRST()  (GPIOA->ODR |= SSD_SELECT)
#define __SELECT_SECOND() (GPIOA->BRR |= SSD_SELECT)

#define __CLEAR_DIGITS()  (GPIOA->BRR |= 0x1873)

#define EMPTY             (20) // depends on the LOOKUP ARRAY
#define MINUS             (19) // depends on the LOOKUP ARRAY

typedef enum {
	IDLE, ON, OFF
} state_t;


// global variables. should be declared volatile
global float disp = 0.0;
global uint8_t digits[4];
global state_t state = IDLE;


const uint32_t LOOKUP[21] = {
		    //    BC    GAF  ED  // backwards
		    //    EF    GDC  BA
	0x1833, //	0b11____011__11; // 0
	0x1800, //	0b11____000__00; // 1
	0x1063, //	0b10____110__11; // 2
	0x1861, //	0b11____110__01; // 3
	0x1850, //	0b11____101__00; // 4
	0x0871, //	0b01____111__01; // 5
	0x0873, //	0b01____111__11; // 6
	0x1820, //	0b11____010__00; // 7
	0x1873, //	0b11____111__11; // 8
	0x1871, //	0b11____111__01; // 9
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000
};

//	0b11____111__11; // A
//	0b11____111__11; // B
//	0b11____111__11; // C
//	0b11____111__11; // D
//	0b11____111__11; // E
//	0b11____111__11; // F
//	0b11____111__11; // o
//	0b11____111__11; // u
//	0b11____111__11; // L
//	0b11____111__11; // -
//	0b00____000__00; // *

void setup_SSD() {
	RCC->IOPENR |= (1U << 0); // Enable GPIOA

    /* Setup PA0,1,4,5,6,7,11,12 as output */
    GPIOA->MODER &= ~(0x03C0FF0F);
    GPIOA->MODER |= (0x01405505);

}

void setup_TIM3() {
	// enable TIM3 clock (bit1)
	RCC->APBENR1 |= (1U << 1);

	TIM3->CR1 = 0;
	TIM3->CNT = 0;
	TIM3->CCR1 = 0;
	TIM3->DIER |= (1U << 0); // UIE
	TIM3->PSC = 47;
	TIM3->ARR = 1000; // 125 Hz

	TIM3->CR1 |= (1 << 0); // enable TIM6

	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn,0);

}
void setup_TIM2() {
	// enable TIM2 clock (bit0)
	RCC->APBENR1 |= (1U << 0);

	TIM2->CR1 = 0;
	TIM2->CNT = 0;
	TIM2->CCR1 = 0;
	TIM2->DIER |= (1U << 0); // UIE
	TIM2->PSC = 1999;
	TIM2->ARR = 1000; // 125 Hz

	TIM2->CR1 |= (1 << 0); // enable TIM2

	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn,0);

}

int32_t f2i(float f) {
	//f = -19.412321451234
	int64_t a = (int64_t)(f * 1000.0);
	//a = -19412
	if (a < 0) {
		while(a >= MIN_DISP) {
			a = (int64_t)(a / 10);
		}
	} else {
		while (a > MAX_DISP) {
			a = (int64_t)(a / 10);
		}
	}
	return (int32_t)a;
}


uint8_t f2dot(float f) {
	//f = -19.412321451234
	uint8_t dot = 0;

	if (f < MIN_DISP) {
		return 0;
	} else if (f > MAX_DISP) {
		return 0;
	}

	while(f > 0 && f < MAX_DISP) {
		f *= 10;
		++dot;
	}

	while(f < 0 && f > MIN_DISP) {
		f *= 10;
		++dot;
	}

	return dot;
}


// will change global digits variable
void updateDigits() {
	float a = disp; // buffer disp

	uint8_t neg = 0;
	uint8_t dot = f2dot(a); // find dot
	int32_t x = f2i(a); // find integer

	if (x < 0) {
		x *= -1;
		neg = 1;
	}

	switch(state) {

		case ON:
			// update digits
			if (x > 1000) {
				digits[3] = (uint8_t)(x / 1000);
				x = x - digits[3] * 1000;
			} else {
				if (neg) {
					digits[3] = MINUS;
				} else {
					digits[3] = EMPTY;
				}
			}
			if (x > 100) {
				digits[2] = (uint8_t)(x / 100);
				x = x - digits[2] * 100;
			} else {
				digits[2] = EMPTY;
			}
			if (x > 10) {
				digits[1] = (uint8_t)(x / 10);
				x = x - digits[1] * 10;
			} else {
				digits[1] = EMPTY;
			}
			digits[0] = (uint8_t)(x);
			break;

		case OFF:
			for(size_t i=0; i<4; ++i) {
				digits[i] = EMPTY;
			}
			break;

		default:
		case IDLE:
			digits[0] = 0;
			digits[1] = 0;
			digits[2] = 0;
			digits[3] = 7;
			break;

	}

}


void displayNumber(uint8_t a) {
	__CLEAR_DIGITS();
	GPIOA->ODR |= LOOKUP[a];
}


// should run at 8ms
void TIM3_IRQHandler() {

	static int c = 0;

	switch(c) {
	case 0:
		// select first digit
		__SELECT_FIRST();
		// display first digit
		displayNumber(digits[0]);
		++c;
		break;
	case 1:
		// select second digit
		__SELECT_SECOND();
		// display second digit
		displayNumber(digits[1]);
		//++c;
		c = 0;
		break;
//	case 2:
//		// display third digit
//		__SELECT_THIRD();
//		// select third digit
//		displayNumber(digits[2]);
//		++c;
//		break;
//	case 3:
//		// display forth digit
//		__SELECT_FORTH();
//		// select forth digit
//		displayNumber(digits[3]);
//		c = 0;
//		break;
	}

	// clear Timer ISR
	TIM3->SR &= ~(1U << 0);

}


// should run at 250 ms
void TIM2_IRQHandler() {
	if (--disp < 0) {
		disp = MAX_DISP;
	}
	updateDigits();
	TIM2->SR &= ~(1U << 0);
}


int main(void) {

	BSP_system_init();
	setup_UART();
	setup_SSD();
	setup_TIM2();
	setup_TIM3();

	state = ON;

	for(;;) {}

    return 0;
}



