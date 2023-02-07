#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"
#include "uint256.c"

int main(){
    uint64_t a[4] = {1, 0, 0, 0};
    UInt256 test = uint256_create(a);
    test = uint256_leftshift(test, 64);
    char *str = uint256_format_as_hex(test);
    printf("%s\n", str);
    return 0;
}