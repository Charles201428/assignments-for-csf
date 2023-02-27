/*
 * Hexdump main function, assembly language version
 * CSF Assignment 2
 * Rongrong Liu
 * rliu39@jhu.edu
 */
	.section .rodata
sSpace:  		.string " "
sTripleSpace: 	.string "   "
sColon:			.string ": "
sNewLine:		.string "\n"

	.section .bss
data_buf:		.space 17
hex_buf:		.space 3
pos_buf:		.space 9


	.section .text

	.globl main
main:
	pushq %rbx				/* stores result of hexread */
	pushq %r12				/* stores offset value */
	pushq %r13				/* for loop index counter */

	movq $0, %r12			/* initialize offset value = 0 */
	movq $data_buf, %rdi	/* move data_buf to first parameter */
	call hex_read			
	movq %rax, %rbx			/* move the result to %rbx */

.LloopWhile:
	cmpq $0, %rbx			/* if hex_read returns > 0, continue looping */
	jle .LloopWhileDone		/* jump to end if returns <=0 */
	movq %r12, %rdi			/* move offset value to the first parameter */
	movq $pos_buf, %rsi		/* move pos_buf to the second parameter */
	call hex_format_offset	
	movq $pos_buf, %rdi		/* move pos_buf to the first parameter */
	call hex_write_string
	movq $sColon, %rdi		/* move ": " to the first parameter */
	call hex_write_string
	movq $0, %r13			/* initialize counter = 0 */
	jmp .LprintHexCond		/* begin for loop */

.LprintHexTop:
	movq $data_buf, %r10 	/* move data_buf to a register to access individual element */
	movq $0, %rdi			/* clear %rdi */
	movb (%r10, %r13), %dil	/* move data_buf[index] to the first parameter */
	movq $hex_buf, %rsi		/* move hex_buf to the second parameter */
	call hex_format_byte_as_hex
	movq $hex_buf, %rdi		/* move hex_buf to be printed */
	call hex_write_string
	movq $sSpace, %rdi		/* move a space to be printed */
	call hex_write_string
	movq $data_buf, %r10 	/* move data_buf to a register to access individual element */
	movq $0, %rdi			/* clear %rdi */
	movb (%r10, %r13), %dil	/* move data_buf[index] to the first parameter */
	call hex_to_printable
	movb %al, (%r10, %r13)	/* move the converted printable character back to data_buf[index] */
	inc %r12				/* increment offset value */
	inc %r13				/* increment index */

.LprintHexCond:
	cmp %rbx, %r13
	jl .LprintHexTop

	movq $0, %r13			/* initialize counter = 0 */

	jmp .LprintSpaceCond
.LprintSpaceTop:
	movq $sTripleSpace, %rdi	/* move three spaces to be written */
	call hex_write_string
	inc %r13				/* increment counter */

.LprintSpaceCond:
	movq $16, %r11			/* set %r11 = 16 */
	subq %rbx, %r11			/* %r11 = 16 - number of chars read */
	cmp %r11, %r13			/* compare index to 16 - size */
	jl .LprintSpaceTop		/* keep iterating if index < 16-size*/

	movq $sSpace, %rdi		/* write an additional space */
	call hex_write_string
	movq $data_buf, %rdi	/* write data */
	call hex_write_string
	movq $sNewLine, %rdi		/* write new line */
	call hex_write_string

	movq $data_buf, %rdi	/* move data_buf to first parameter to read new string */
	call hex_read
	movq %rax, %rbx			/* move the result to %rbx */

	jmp .LloopWhile			/* jump back to the top of while loop */

.LloopWhileDone:
	movq $0, %rax			/* main returns 0*/
	popq %r13
	popq %r12				/* pop callee-saved values from stack */
	popq %rbx
	ret

/* vim:ft=gas:
 */
