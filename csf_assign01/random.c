#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"
#include "uint256.c"

int main()
{
    char *str = "123456789abcde510";
    UInt256 num = uint256_create_from_hex(str);
    uint64_t data0 = num.data[0];
    uint64_t data1 = num.data[1];
    uint64_t data2 = num.data[2];
    uint64_t data3 = num.data[3];
    printf("%lx %lx %lx %lx\n", data0, data1, data2, data3);
    //123456789abcde510
    //23456789abcde510
 
    return 0;
}