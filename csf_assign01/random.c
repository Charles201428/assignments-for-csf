#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"
#include "uint256.c"

int main()
{
    char str[50] = "geeksforgeeks";
 
    printf("The given string is =%s\n", str);
 
    printf("After reversing string is =%s", strrev(str));
 
    return 0;
}