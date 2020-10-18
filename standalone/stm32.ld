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

	.data :
	{
		__data_start__ = .;
		*(.data*)         /* Read-write initialized data */
		. = ALIGN(4);
		__data_end__ = .;
	} >RAM AT> ROM

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)          /* Read-write zero initialized data */
		. = ALIGN(4);
		__bss_end__ = .;
	} >RAM
}
