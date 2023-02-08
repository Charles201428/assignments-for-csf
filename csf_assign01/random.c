#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"
#include "uint256.c"

int main(){
    UInt256 a = uint256_create_from_hex("abcde10000abce237946139572abc");
    long data0 = a.data[0];
    long data1 = a.data[1];
    long data2 = a.data[2];
    long data3 = a.data[3];
    printf("%lx %lx %lx %lx\n", data0, data1, data2, data3);
    return 0;
}