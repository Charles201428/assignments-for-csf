#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"
#include "uint256.c"

int main(){
    uint64_t a[4] = {1324, 0, 0, 0};
    UInt256 val = uint256_create(a);
    //val = uint256_leftshift(val, 10);
    UInt256 left, right, result;
    left.data[0] = 0x63f23766d1391782UL;
  left.data[1] = 0x761544a98b82abcUL;
  left.data[2] = 0x0UL;
  left.data[3] = 0x0UL;
  right.data[0] = 0x484c32d955a47a2fUL;
  right.data[1] = 0x14bf658bd8053a9UL;
  right.data[2] = 0x0UL;
  right.data[3] = 0x0UL;
  result = uint256_mul(left, right);
    printf("%lx\n", result.data[1]);
    
    
    char *str = uint256_format_as_hex(val);
    //printf("%s\n", str);
    uint64_t data0 = val.data[0];
    uint64_t data1 = val.data[1];
    uint64_t data2 = val.data[2];
    uint64_t data3 = val.data[3];
    printf("%lx %lx %lx %lx\n", data0, data1, data2, data3);
    //printf("%s\n", str);
    int r1 = 1234;
    uint64_t r2 = r1 & ~(~0UL << 4);
    printf("this is the int: %lx\n", r2);
    for (int i = 0; i < 256; i++){
        UInt256 new = uint256_leftshift(val, i);
        char *hexi = uint256_format_as_hex(new);
        if (strcmp(hexi, "0") == 0){
            printf("the nth entry: %d\n", i);
            printf("current data0: %d\n", new.data[3]);
        }
    }
}