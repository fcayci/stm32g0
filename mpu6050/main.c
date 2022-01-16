/*
 * main.c
 *
 * author: Furkan Cayci
 */

#include "bsp.h"

#define MPU6050_ADDRESS      0x68

#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40
#define MPU6050_TEMP_OUT_H   0x41
#define MPU6050_TEMP_OUT_L   0x42
#define MPU6050_GYRO_XOUT_H  0x43
#define MPU6050_GYRO_XOUT_L  0x44
#define MPU6050_GYRO_YOUT_H  0x45
#define MPU6050_GYRO_YOUT_L  0x46
#define MPU6050_GYRO_ZOUT_H  0x47
#define MPU6050_GYRO_ZOUT_L  0x48


void I2C1_IRQHandler(void) {

}

void write_I2C(uint8_t deviceAddr, uint8_t* buf, uint32_t num) {

    // send address to write in write mode
    I2C1->CR2 = 0;               // clear CR2
    I2C1->CR2 |= (uint32_t)(deviceAddr << 1);   // Write slave address SADD
    I2C1->CR2 |= (2U << 16);     // number of bytes to tx NBYTES
    I2C1->CR2 |= (1U << 25);     // AUTOEND
    I2C1->CR2 |= (1U << 13);     // generate START

    int i = 0;
    while(num > 0) {
        while (!(I2C1->ISR & (1U << 1))); // wait until TXIS
		I2C1->TXDR = buf[i++];
		--num;
    }

//    while (!(I2C1->ISR & (1U << 5))); // wait until STOPF
//    I2C1->ICR |= (1U << 5); // clear STOPF flag
}

uint8_t read_I2C(uint8_t deviceAddr, uint8_t regAddr, uint32_t num) {
	uint8_t data; // FIXME this will be included with the function call

    // send address to read in write mode
    I2C1->CR2 = 0;               // clear CR2
    I2C1->CR2 |= ((uint32_t)deviceAddr << 1);   // Write slave address SADD
    I2C1->CR2 |= (1U << 16);     // number of bytes to write NBYTES

    I2C1->CR2 |= (1U << 13);     // generate START
    while (!(I2C1->ISR & (1U << 1))); // wait until TXIS

    I2C1->TXDR = regAddr;
    while (!(I2C1->ISR & (1U << 6))); // wait until TC

    // wait on BUSY FLAG
    // clear BUSY Flag
    if (num > 255) {
    	num = 255; // FIXME. Will need to play with RELOAD
    }

	// read address
	I2C1->CR2 = 0;               // clear CR2
	I2C1->CR2 |= (uint32_t)(deviceAddr << 1);   // Write slave address SADD
	I2C1->CR2 |= (1U << 10);     // READ mode

	I2C1->CR2 |= ((uint32_t)num << 16);     // number of bytes to read NBYTES
	I2C1->CR2 |= (1U << 25);     // AUTOEND

	I2C1->CR2 |= (1U << 13);     // generate START

	while(num > 0) {
		while (!(I2C1->ISR & (1U << 2))); // wait until RXNE
		data = (uint8_t)I2C1->RXDR;
		--num;
    }

//    while (!(I2C1->ISR & (1U << 5))); // wait until STOPF
//    I2C1->ICR |= (1U << 5); // clear STOPF flag

	return data;
}

void setup_I2C(void) {

	/* Setup PB8-9 as AF6 for I2C1 SDA/SCL */

    /* Enable GPIOB clock */
    RCC->IOPENR   |= (1U << 1);

    // set pin modes as alternate mode 6

    /* Setup PB8 as alternate function */
    GPIOB->MODER  &= ~(3U << 2*8);
    GPIOB->MODER  |=  (2U << 2*8);
    GPIOB->OTYPER |=  (1U << 8); // open-drain
    GPIOB->AFR[1] |=  (6U << 4*0);

    /* Setup PB9 as alternate function */
    GPIOB->MODER  &= ~(3U << 2*9);
    GPIOB->MODER  |=  (2U << 2*9);
    GPIOB->OTYPER |=  (1U << 9); // open-drain
    GPIOB->AFR[1] |=  (6U << 4*1);

    /* Setup I2C1 */

    /* Enable I2C1 clock */
    RCC->APBENR1 |= (1U << 21);

    I2C1->CR1 = 0;
    //I2C1->CR1 |= (1 << 2); // Receive interrupt enable
    I2C1->CR1 |= (1 << 7); // Error interrupts enable
    I2C2->CR2 |= (1 << 25); // enable autoend
    I2C2->CR2 |= (1 << 15); // enable nack

    // from table 157. pg 912, timings for 16 Mhz standard-mode 100 khz
    I2C1->TIMINGR |= (3 << 28);   // PRESC  3
    I2C1->TIMINGR |= (0x2 << 16); // SDADEL 0x2
    I2C1->TIMINGR |= (0x4 << 20); // SCLDEL 0x4
    I2C1->TIMINGR |= (0xF << 8);  // SCLH   0xF
    I2C1->TIMINGR |= (0x13 << 0); // SCLL   0x13

    I2C1->ICR = 0x3F38; // clear ISR
    I2C1->CR1 |= (1 << 0); // enable

    NVIC_EnableIRQ(I2C1_IRQn);
    NVIC_SetPriority(I2C1_IRQn, 0);

}

int main(void) {

	BSP_system_init();
	setup_UART();
	setup_I2C();

    uint8_t ret = read_I2C(MPU6050_ADDRESS, MPU6050_WHO_AM_I, 1);
	printf("WHO AM I: %x\r\n", ret);

    ret = read_I2C(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 1);
	printf("PWR MNGT: %x\r\n", ret);

    // by default sleep is active, first reset, then clear sleep mode
    //write_I2C(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 0x80);
    // clock is from pll with x gyro
	//write_I2C(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 0x01);
    // enable
	uint8_t buf[2];
	buf[0] = MPU6050_PWR_MGMT_1;
	buf[1] = 0x80; // reset
	write_I2C(MPU6050_ADDRESS, buf, 2);

	delay_ms(1000);

    ret = read_I2C(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 1);
	printf("PWR MNGT: %x\r\n", ret);

	buf[0] = MPU6050_PWR_MGMT_1;
	buf[1] = 0x00; // enable
	write_I2C(MPU6050_ADDRESS, buf, 2);

	delay_ms(1000);

    ret = read_I2C(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 1);
	printf("PWR MNGT: %x\r\n", ret);

	double ax;
	for(;;) {
		uint8_t ah = read_I2C(MPU6050_ADDRESS, MPU6050_GYRO_ZOUT_H, 1);
		uint8_t al = read_I2C(MPU6050_ADDRESS, MPU6050_GYRO_ZOUT_L, 1);
		ax = (double)((ah << 8) | al) / 131.0;
		printf("%f\r\n", ax);
		delay_ms(1);
    }

    return 0;
}
