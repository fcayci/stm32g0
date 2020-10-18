/* basic linker script */

MEMORY
{
	ROM  (rx) : ORIGIN = 0x08000000, LENGTH = 64K
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH =  8K
}

ENTRY(Reset_Handler)

/* end of RAM */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
	.text :
	{
		KEEP(*(.vectors)) /* Vector table */
		*(.text*)         /* Program code */
		*(.rodata*)       /* Read only data */
	} >ROM

	__etext = ALIGN (4);
	_sidata = LOADADDR(.data);

	.data :
	{
		_sdata = .;
		*(.data*)         /* Read-write initialized data */
		. = ALIGN(4);
		_edata = .;
	} >RAM AT> ROM

	.bss :
	{
		. = ALIGN(4);
		_sbss = .;
		*(.bss*)          /* Read-write zero initialized data */
		. = ALIGN(4);
		_ebss = .;
	} >RAM
}
