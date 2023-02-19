/* Assembly language implementation of hexdump functions */

 .section .rodata

 .section .text


/* vim:ft=gas:
 */
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
 /* TODO: implement */
 ret