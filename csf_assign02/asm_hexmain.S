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

	.globl printoffset /*same function in as in cmain */
.printoffset:
	call hex_format_offset
	movq %rsi, %rdi  /* move offset to first argument */
	call hex_write_string
	movq sColon, %rdi /*print out colon */
	call hex_write_string
	ret
	
	
	.globl printdata /*same function as in cmain */
.printdata:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq %rdi, (%rbp) //store %rdi 
	movq sSpace, %rdi /* wanna print space */
	call hex_write_string
	movq (%rbp), %rdi /*print data_buf */
	call hex_write_string
	movq sNewLine, %rdi /*print new line */
	call hex_write_string
	ret

	.globl printsupplement /*same function as in cmain */
.printsupplement:
	push %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl %edi, -20(%rbp) // store the input index
	movl $15, %eax // this get the last four digits of index
	testl %eax, %eax //this test whether %eax is 0;
	//if yes, then it's a multiple of 16, and thus if statement is not met
	je .theEnd
	movl $0, -4(%rbp) // counter
.LoopCase:
	movl $16, %r10d 
	subl %eax, %r10d //r10 = 16 - index%16
    cmpl %r10d, -4(%rbp) //if i < 16 - index%16
    jge  .theEnd
	movl sTripleSpace, %edi //print three space
	call hex_write_string
	movl -20(%rbp), %edi //restore edi
    addl $1, -4(%rbp) //counter increments
    jmp  .LoopCase
.theEnd:
    add $16, %rsp
    popq %rbp
    ret

	.globl printloop
.printloop:
        pushq   %rbp
        movq    %rsp, %rbp
		subq 	$16, %rsp
        movl    %edi, -20(%rbp)
        movq    %rsi, -32(%rbp)
        movq    %rdx, -40(%rbp)
        movl    $0, %r10d //r10 = counter
.printlooploopcase:
        movl    %r10d, %eax
        cmpl    -20(%rbp), %eax
        jge     .printloopend
        movq 	(%rsi, %r10), %rdi //r11 = data[i]
		movq 	%rdx, %rsi //rsi = hex
		call hex_format_byte_as_hex
		movq 	%rdx, %rdi //rdi = hex
		call hex_write_string
		movq 	sSpace, %rdi //rdi = 3 spacces
		call hex_write_string
		movl    -20(%rbp), %edi
        movq    -32(%rbp), %rsi
        movq    -40(%rbp), %rdx  //restore three arguments
        addl    $1, %r10d
        jmp     .printlooploopcase
.printloopend:
        addq $16, %rsp
        popq    %rbp
        ret



	.globl main
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $64, %rsp
        movq    $0, -8(%rbp)
        leaq    -48(%rbp), %rax
        movq    %rax, %rdi
        call    hex_read
        movl    %eax, -12(%rbp)
.L35:
        cmpl    $0, -12(%rbp)
        jle     .L32
        movq    -8(%rbp), %rax
        movl    %eax, %edx
        leaq    -25(%rbp), %rax
        movq    %rax, %rsi
        movl    %edx, %edi
        call    printoffset
        leaq    -51(%rbp), %rdx
        leaq    -48(%rbp), %rcx
        movl    -12(%rbp), %eax
        movq    %rcx, %rsi
        movl    %eax, %edi
        call    printloop
        movl    $0, -16(%rbp)
.L34:
        movl    -16(%rbp), %eax
        cmpl    -12(%rbp), %eax
        jge     .L33
        movl    -16(%rbp), %eax
        cltq
        movzbl  -48(%rbp,%rax), %eax
        movzbl  %al, %eax
        movl    %eax, %edi
        call    hex_to_printable
        movl    -16(%rbp), %edx
        movslq  %edx, %rdx
        movb    %al, -48(%rbp,%rdx)
        addq    $1, -8(%rbp)
        addl    $1, -16(%rbp)
        jmp     .L34
.L33:
        movq    -8(%rbp), %rax
        movl    %eax, %edi
        call    printsupplement
        leaq    -48(%rbp), %rax
        movq    %rax, %rdi
        call    printdata
        leaq    -48(%rbp), %rax
        movq    %rax, %rdi
        call    hex_read
        movl    %eax, -12(%rbp)
        jmp     .L35
.L32:
        movl    $0, %eax
        leave
        ret

/* vim:ft=gas:
 */
