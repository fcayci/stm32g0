/*
 * asm.s
 *
 * author: Furkan Cayci
 *
 * description:
 *   Connected SSD pins:
 *   A0   - G
 *   A1   - F
 *   A4   - E
 *   A5   - D
 *   A12  - C
 *   A11  - B
 *   A6   - AA
 *   A7   - CAT
 */


.syntax unified
.cpu cortex-m0plus
.fpu softvfp
.thumb


/* make linker see this */
.global Reset_Handler

/* get these from linker script */
.word _sdata
.word _edata
.word _sbss
.word _ebss


/* define peripheral addresses from RM0444 page 57, Tables 3-4 */
.equ RCC_BASE,         (0x40021000)          // RCC base address
.equ RCC_IOPENR,       (RCC_BASE   + (0x34)) // RCC IOPENR register offset

.equ GPIOA_BASE,       (0x50000000)          // GPIOA base address
.equ GPIOA_MODER,      (GPIOA_BASE + (0x00)) // GPIOA MODER register offset
.equ GPIOA_ODR,        (GPIOA_BASE + (0x14)) // GPIOA ODR register offset

.equ GPIOB_BASE,       (0x50000400)          // GPIOB base address
.equ GPIOB_MODER,      (GPIOB_BASE + (0x00)) // GPIOB MODER register offset
.equ GPIOB_IDR,        (GPIOB_BASE + (0x10)) // GPIOB IDR register offset
.equ GPIOB_ODR,        (GPIOB_BASE + (0x14)) // GPIOB ODR register offset

.equ GPIOC_BASE,       (0x50000800)          // GPIOC base address
.equ GPIOC_MODER,      (GPIOC_BASE + (0x00)) // GPIOC MODER register offset
.equ GPIOC_ODR,        (GPIOC_BASE + (0x14)) // GPIOC ODR register offset

.equ COUNTMAX,        99
.equ BDELAY,          20
.equ SSDDELAY,        10000


/* vector table, +1 thumb mode */
.section .vectors
vector_table:
	.word _estack             /*     Stack pointer */
	.word Reset_Handler +1    /*     Reset handler */
	.word Default_Handler +1  /*       NMI handler */
	.word Default_Handler +1  /* HardFault handler */
	/* add rest of them here if needed */


/* reset handler */
.section .text
Reset_Handler:
	/* set stack pointer */
	ldr r0, =_estack
	mov sp, r0

	/* initialize data and bss
	 * not necessary for rom only code
	 * */
	bl init_data
	/* call main */
	bl main
	/* trap if returned */
	b .


/* initialize data and bss sections */
.section .text
init_data:

	/* copy rom to ram */
	ldr r0, =_sdata
	ldr r1, =_edata
	ldr r2, =_sidata
	movs r3, #0
	b LoopCopyDataInit

	CopyDataInit:
		ldr r4, [r2, r3]
		str r4, [r0, r3]
		adds r3, r3, #4

	LoopCopyDataInit:
		adds r4, r0, r3
		cmp r4, r1
		bcc CopyDataInit

	/* zero bss */
	ldr r2, =_sbss
	ldr r4, =_ebss
	movs r3, #0
	b LoopFillZerobss

	FillZerobss:
		str  r3, [r2]
		adds r2, r2, #4

	LoopFillZerobss:
		cmp r2, r4
		bcc FillZerobss

	bx lr


/* default handler */
.section .text
Default_Handler:
	b Default_Handler


/* main function */
.section .text
main:

	/*****************************
	 * Enable GPIOA / GPIOB / GPIOC clocks
	 *****************************/
	ldr  r3, =RCC_IOPENR
	ldr  r2, [r3]
	movs r1, #0x27
	orrs r2, r2, r1
	str  r2, [r3]


	/*****************************
	 * PC6 - On-Board LED
	 * Make output, set high
	 *****************************/
	ldr  r3, =GPIOC_MODER
	ldr  r2, [r3]
	ldr  r1, =#0x3000
	bics r2, r2, r1
	ldr  r1, =#0x1000
	orrs r2, r2, r1
	str  r2, [r3]

	/* turn on led connected to C6 in ODR */
	ldr  r3, =GPIOC_ODR
	ldr  r2, [r3]
	movs r1, #0x40
	orrs r2, r2, r1
	str  r2, [r3]


	/*****************************
	 * PB3 - External Button
	 * Make input
	 *****************************/
	ldr  r3, =GPIOB_MODER
	ldr  r2, [r3]
	ldr  r1, =#0xC0
	bics r2, r2, r1
	str  r2, [r3]


	/*****************************
	 * PA 0, 1, 4, 5, 12, 11, 6
	 * PA 7 is CAT
	 * Make output, set high
	 *****************************/
	ldr  r3, =GPIOA_MODER
	ldr  r2, [r3]
	ldr  r1, =#0x03C0FF0F
	bics r2, r2, r1
	ldr  r1, =#0x01405505
	orrs r2, r2, r1
	str  r2, [r3]

	/* turn them off */
	ldr  r3, =GPIOA_ODR
	ldr  r2, [r3]
	ldr  r1, =#0x1873
	bics r2, r2, r1
	str  r2, [r3]


/*****************************
* main while loop
*****************************/
infiloop:
	/* number to be displayed */
	ldr  r0, =COUNTMAX
	ldr  r1, =BDELAY
	movs r7, #0x1 // holds mode
	push {r1}
	loop:
		pop  {r1}
		bl   process
		push {r1}
		bl   display

		movs r1, #0
		cmp  r0, r1
		bge  loop
		b    infiloop

process:
	// read button
	// if pressed go to wait
	// if not go to decrement
	// r0 holds counter
	// r1 holds delay counter
	push {r2, r3, lr}

	ldr  r3, =GPIOB_IDR
	ldr  r3, [r3]
	lsrs r3, #4
	bcs  change_mode

	check_display:
		// check if should display
		movs r2, #0
		cmp  r1, r2
		bne  halt
		ldr  r1, =BDELAY

		// check if mode 0
		cmp  r7, r2
		beq  halt
		subs r0, r0, #1 // decrement number
	halt:
		subs r1, r1, #1
		pop  {r2, r3, pc}

	change_mode:
		push {r0}
		movs r3, #0x1
		eors r7, r7, r3

		/* show mode on on-board LED */
		ldr  r3, =GPIOC_ODR
		ldr  r2, [r3]
		movs r0, #0x40
		bics r2, r2, r0
		movs r0, #6
		push {r7}
		lsls r7, r7, r0
		orrs r2, r2, r7
		pop  {r7}
		str  r2, [r3]

		/* restart counter delay */
		ldr r1, =BDELAY
		pop {r0}
		b   check_display


display:
// r0 holds displayed number
	push {r0-r3, lr} // save displayed number

	// RIGHT DIGIT
	// activate right ssd
	ldr  r3, =GPIOA_ODR
	ldr  r2, [r3]
	movs r1, #0x80
	orrs r2, r2, r1
	str  r2, [r3]

	bl   digits
	push {r0}
	ldr  r0, =SSDDELAY
	bl   leddelay
	pop  {r0}

	// LEFT DIGIT
	// activate left ssd
	ldr  r3, =GPIOA_ODR
	ldr  r2, [r3]
	movs r1, #0x80
	bics r2, r2, r1
	str  r2, [r3]

	bl   digits
	push {r0}
	ldr  r0, =SSDDELAY
	bl   leddelay
	pop  {r0}

	pop  {r0-r3, pc}


// r0 holds the whole number
// returns r0 as the remaining number
digits:
	push {lr}
	// r0 holds the number to be displayed
	// calculate r0 / r1
	movs r1, #10
	bl div
	// r0 = 4 will display this
	// r1 = 1 will display next

	// first read port
	ldr  r3, =GPIOA_ODR
	ldr  r2, [r3]
	// clear all numbers
	ldr  r4, =#0x1873
	bics r2, r2, r4
	// read number from array based on r0
	ldr  r4, =NUMBERS
	lsls r0, r0, #2 // word align
	ldr  r4, [r4, r0]
	orrs r2, r2, r4
	// write to port
	str  r2, [r3]
	movs r0, r1
	pop  {pc}


// led delay between ssds
// expects r0 to be set
leddelay:
	subs r0, r0, #1
	bne  leddelay
	bx   lr

// divison procedure
// does r0/r1 operation
// returns r0 = r0 % r1
// returns r1 = int(r0 / r1)
div:
	push {r2}
	movs r2, #0

	divloop:
		cmp  r0, r1
		blt  done
		subs r0, r0, r1
		adds r2, r2, #1
		b    divloop

	done:
		movs r1, r2
		pop  {r2}
		bx   lr


.align
NUMBERS:
	.word 0x1872 // 0
	.word 0x0012 // 1
	.word 0x0871 // 2
	.word 0x0073 // 3
	.word 0x1013 // 4
	.word 0x1063 // 5
	.word 0x1863 // 6
	.word 0x0032 // 7
	.word 0x1873 // 8
	.word 0x1073 // 9
