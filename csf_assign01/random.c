#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"
#include "uint256.c"

int main(){
    uint64_t a[4] = {1, 1, 0, 0};
    UInt256 val = uint256_create(a);
    val = uint256_leftshift(val, 160);
    
    char *str = uint256_format_as_hex(val);
    //printf("%s\n", str);
    uint64_t data0 = val.data[0];
    uint64_t data1 = val.data[1];
    uint64_t data2 = val.data[2];
    uint64_t data3 = val.data[3];
    printf("%lx %lx %lx %lx\n", data0, data1, data2, data3);
    printf("%s\n", str);
    
}