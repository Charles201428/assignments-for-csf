/* Assembly language implementation of hexdump functions */

 .section .rodata

 .section .text


/* vim:ft=gas:
 */

    .globl hex_read

hex_read:
	pushq %rbx                  /* push callee-saved values onto the stack */
    subq $8, %rsp				/* align stack */
	movq %rdi, %rbx				/* copy buffer pointer */
    movq %rdi, %rsi				/* move buffer pointer to the second parameter */
    movq $16, %rdx 				/* num of bytes to read */
	movq $0, %rdi 				/* 0: set file descriptor = stdin */
    call read
	movq $0, (%rax, %rbx) 		/* set the final char to '\0' */
    addq $8, %rsp 
	popq %rbx                   /* push callee-saved values from the stack */
    ret

    .globl hex_write_string
hex_write_string:
    movq %rsp, %rbp
    subq $24, %rsp /* adjust stack pointer */
    movq $0, %r10 /* set the char pointed by counter to 0 */
    movq $0, %rdx /* set counter to 0 */
.Lcount:
    movb (%rdi, %rdx), %r10b /* set value of the char pointed by counter */
    cmpb $0, %r10b /* is \0 = the char pointed by counter? */
    je .Lout /* If yes, counter is the length of array, loop ends */
    incq %rdx   /* If no, increase counter by 1 */
    jmp .Lcount /* test the next char */
.Lout:
    leaq (%rdi), %rsi /* assign address of the array to 2nd argument*/
    movq $1, %rdi /* write to stdout */
    call write  /* write the string to stdout */
    addq $24, %rsp /* restore stack pointer */
    ret


conbyte_to_hex:     /* this function will help to convert the byte to the hex */
    subq $8, %rsp    /* we align the stack here */ 
    cmpq $10, %rdi    /* check whether it's larger than 10 or smaller than 10 */
    jl .LLargerTen     /* jump if less than 10 */
    addq $87, %rdi    /* when the input is more than 10, we just add the a - 19 to it*/
    jmp .LWrapp     /* jumping right now*/
    
    .LLargerTen:
    addq $48, %rdi        /* since here is when the input is less than 10, adding the 0 */

    .LWrapp:      
    movq %rdi, %rax    /* we wrapped up the function */
    addq $8, %rsp      /* put the rsp to the original place */
    ret

 .globl hex_to_printable
hex_to_printable:
 subq $8, %rsp    /* we align the stack here  */
 cmp $32, %rdi    /*  check whether it's larger than 32 or smaller than 32*/
 jl .LUnableToPrint       /* we can't print the value less than 32*/
 cmpq $126, %rdi       /* make the comparisopn*/
 jg .LUnableToPrint       /* value larger than 126 isn't printable*/
 movq %rdi, %rax       /* move it to the return value if it's between 32 to 126 */
 jmp .LWrappingUp   /* wrapping the function */     
 
.LUnableToPrint:
 movq $46, %rax        /* return value = 46 */

.LWrappingUp:
 addq $8, %rsp               /* put the rsp to the original place */
 ret


 .globl hex_format_byte_as_hex
hex_format_byte_as_hex:
    pushq %rbp      
    subq $16, %rsp        /* we align the stack here */   
    movq %rsp, %rbp         /* here is aligning the stack as well */   
    movq %rdi, 0(%rbp)     
    movq %rdi, 8(%rbp)
    jmp .LDealWithFirst   /* here we start to convert the first four bits to the hex and put it in the index 0 of the array */
   
.LDealWithFirst:

    shrl $4, 0(%rbp)       /*for here, we shift the four bits to the right and get the first four bits*/
    movq 0(%rbp), %rdi    /*for here, we shift the four bits to the right and get the first four bits*/ 
        call conbyte_to_hex    /*call the conversion function to do the function*/
    movq %rax, 0(%rsi, 1) 
    movq 8(%rbp), %rdi 
    jmp .LDealWithSecond    /* here we start to convert the second four bits to the hex and put it in the index 0 of the array */
    
.LDealWithSecond:

    shll $4, 0(%rbp)    /*for here, we shift the four bits to the left and get the second four bits*/ 
    subq 0(%rbp), %rdi 
    call conbyte_to_hex   /*call the conversion function to do the function*/
    movq %rax, 1(%rsi, 1) 
    jmp .LDealWithZero    /* here start to adding the last 0 and wraped up the function */
    
.LDealWithZero:
    movq $0, 2(%rsi, 1)   /* move the last 0 */
    addq $16, %rsp       /* put the rsp to the original place */           
    addq $3, %rsi
    popq %rbp
    ret 

.globl hex_format_offset
hex_format_offset:
    pushq %rbp
    pushq %rbx
    subq $16, %rsp
    movq %rsp, %rbp  /*this is just the prelo for stack allignment */
    movq $0, %rbx /*counter for the for loop from 1 to 8 */
    /can't do %rdp = %rdi since rbp is already set to be rsp/
    movq %rdi, (%rbp) /*note that %rdi here is the offset, and we move it to the memory address referenced by %rbx */
    jmp .endcase /*this is the case where the loop endcase */


.loopcase:
    andq $15, %rdi /*this gives last for bit of offset since 15 = 01111 */
    call conbyte_to_hex /*take the %rdi and convert it to a hex*/
    /*now it's in %rax, but it only has 1 byte, thus %al */
    movq $7, %r10 
    subq %rbx, %r10
    movb %al, (%rsi, %r10) /* %rsi = buf, thus buf[-i+7] = %al */
    shrq $4, (%rbp) /*offset right shift 4 bits */
    movq (%rbp), %rdi /*for the next conbyte_to_hex call */
    incq %rbx /* i++ */

.endcase:
    cmpq $8, %rbx /*loop through 1-7 so when %rbx is smaller than 8 we still go back to loop case */
    jl .loopcase
    /*otherwise end the loop and function*/
    movb $0, 8(%rsi) /*each char has 1 byte, and %rsi (buf) has 8 chars, so this add \0 to buf[8] */
    addq $16, %rsp
    popq %rbx
    popq %rbp
    ret 

<<<<<<< HEAD

=======
>>>>>>> bb4c8ca (main finished)
