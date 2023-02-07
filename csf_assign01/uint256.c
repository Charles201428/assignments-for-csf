#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint64_t value.
// Only the least-significant 64 bits are initialized directly,
// all other bits are set to 0.

UInt256 uint256_create_from_u64(uint64_t val) {
  UInt256 result;
  result.data[0] = val;
  result.data[2] = 0;
  result.data[1] = 0;
  result.data[3] = 0;
  return result;
}

// Create a UInt256 value from an array of 4 uint64_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint64_t data[4]) {
  UInt256 result;
  // TODO: implement
  for (int i = 0; i<4; i++){
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  int len = strlen(hex);
  

  //store the rightmost 64 digits of the hex if the length of hex exceeds 64
  

  if (len >64){
    char newchar1[16];
    char newchar2[16];
    char newchar3[16];
    char newchar4[16];
    //only store the first 64 chars and divide them into group per 16 chars
    for (int i = 0; i < 16; i++){
      newchar1[i] = hex[i];
    }
    for (int j = 16; j < 32; j++){
      newchar2[j] = hex[j];
    }
    for (int k = 32; k < 48; k++){
      newchar3[k] = hex[k];
    }
    for (int l = 48; l < 64; l++){
      newchar4[l] = hex[l];
    }
    char *str1;
    char *str2;
    char *str3;
    char *str4;
    //convert each string back to int
    long int4 = strtoul(newchar4, &str4, 16);
    long int3 = strtoul(newchar3, &str3, 16);
    long int2 = strtoul(newchar2, &str2, 16);
    long int1 = strtoul(newchar1, &str1, 16);
    //let's plug those value
    result.data[0] = int4;
    result.data[1] = int3;
    result.data[2] = int2;
    result.data[3] = int1;

  }
  else{
    
    if (len <= 16){
      char *ptr;
      long int0 = strtoul(hex, &ptr, 16);
      result.data[0] = int0;
      result.data[1] = 0;
      result.data[2] = 0;
      result.data[3] = 0;
    }
    else if(len <= 32){
      char newchar1[16];
      char newchar2[len-16];
      for(size_t i =0; i < 16; i++){
        newchar1[i] = hex[i];  
      }
      for (int j = 16; j < len; j++){
        newchar2[j] = hex[j];
      }
      char *ptr0;
      char *ptr1;
      long int0 = strtoul(newchar1, &ptr0, 16);
      long int1 = strtoul(newchar2, &ptr1, 16);
      result.data[0] = int0;
      result.data[1] = int1;
      result.data[2] = 0;
      result.data[3] = 0;
    }
    else if(len <= 48){
      char newchar1[16];
      char newchar2[16];
      char newchar3[len-32];
      for(size_t i =0; i < 16; i++){
        newchar1[i] = hex[i];  
      }
      for (int j = 16; j < 32; j++){
        newchar2[j] = hex[j];
      }
      for (int k = 32; k < len; k++){
        newchar3[k] = hex[k];
      }
      char *ptr0;
      char *ptr1;
      char *ptr2;
      long int0 = strtoul(newchar1, &ptr0, 16);
      long int1 = strtoul(newchar2, &ptr1, 16);
      long int2 = strtoul(newchar3, &ptr2, 16);
      result.data[0] = int0;
      result.data[1] = int1;
      result.data[2] = int2;
      result.data[3] = 0;
    }
    else{
      char newchar1[16];
      char newchar2[16];
      char newchar3[16];
      char newchar4[len-48];
      for(size_t i =0; i < 16; i++){
        newchar1[i] = hex[i];  
      }
      for (int j = 16; j < 32; j++){
        newchar2[j] = hex[j];
      }
      for (int k = 32; k < len; k++){
        newchar3[k] = hex[k];
      }
      for (int l = 48; l < len; l++){
        newchar4[l] = hex[l];
      }
      char *ptr0;
      char *ptr1;
      char *ptr2;
      char *ptr3;
      long int0 = strtoul(newchar1, &ptr0, 16);
      long int1 = strtoul(newchar2, &ptr1, 16);
      long int2 = strtoul(newchar3, &ptr2, 16);
      long int3 = strtoul(newchar4, &ptr3, 16);
      result.data[0] = int0;
      result.data[1] = int1;
      result.data[2] = int2;
      result.data[3] = int3;
    }
    
  }
  
  
 
  

 
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = malloc(sizeof(char)*70);
  hex[0] = '\0';
  uint64_t data0 = val.data[0];
  uint64_t data1 = val.data[1];
  uint64_t data2 = val.data[2];
  uint64_t data3 = val.data[3];
  //if the head uint64 is nonzero
  if (data3 != 0){
    char *last = malloc(sizeof(char)*17);
    last[0] = '\0';
    sprintf(last, "%lx", data3);
    strcat(hex, last);
    free(last);
    for (int i = 2; i > -1; i--){
      char *buf = malloc(sizeof(char)*17);
      buf[0] = '\0';
      sprintf(buf, "%016lx", val.data[i]);
      strcat(hex, buf);
      free(buf);
    }

  }
  else if(data2 != 0){
    char *last = malloc(sizeof(char)*17);
    last[0] = '\0';
    sprintf(last, "%lx", data2);
    strcat(hex, last);
    free(last);
    for (int i = 1; i > -1; i--){
      char *buf = malloc(sizeof(char)*17);
      buf[0] = '\0';
      sprintf(buf, "%016lx", val.data[i]);
      strcat(hex, buf);
      free(buf);
    }
  }
  else if(data1 !=0){
    char *last = malloc(sizeof(char)*17);
    last[0] = '\0';
    sprintf(last, "%lx", data1);
    strcat(hex, last);
    free(last);
    for (int i = 0; i > -1; i--){
      char *buf = malloc(sizeof(char)*17);
      buf[0] = '\0';
      sprintf(buf, "%016lx", val.data[i]);
      strcat(hex, buf);
      free(buf);
    }
  }
  else{
    char *only = malloc(sizeof(char)*17);
    only[0] = '\0';
    sprintf(only, "%lx", data0);
    strcat(hex, only);
    free(only);
  }

  return hex;
}

// Get 64 bits of data from a UInt256 value.
// Index 0 is the least significant 64 bits, index 3 is the most
// significant 64 bits.
uint64_t uint256_get_bits(UInt256 val, unsigned index) {
  uint64_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
 UInt256 sum;
 int carry = 0;
 for (int i = 0; i<4; ++i){
 uint64_t temp = left.data[i] + right.data[i] + carry;
 sum.data[i] = temp;
 if (temp < left.data[i] || temp < right.data[i]){
  carry = 1;
 }
 else{
  carry = 0;
  }
 }
 return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  UInt256 tem;

  for(int i = 0; i < 4; i++){
   tem.data[i] = ~(right.data[i]);
  }
  UInt256 one = uint256_create_from_u64(1UL);
  UInt256 negativeRight = uint256_add(tem, one);
  result = uint256_add(left, negativeRight);
  return result;
}






UInt256 uint256_leftshift(UInt256 val, unsigned shift){
  UInt256 result;
  uint64_t data0 = val.data[0];
  uint64_t data1 = val.data[1];
  uint64_t data2 = val.data[2];
  uint64_t data3 = val.data[3];
  if (shift > 192){
    //this will just move the first 256-shift bits to the last uint64, while other things are filled with zero
    data3 = data0 & ~(~0UL << (256-shift));
    //printf("%d\n", data3);
    data3 = (uint64_t) data3 << (shift - 192);
    
    data0 = 0U;
    data1 = 0U;
    data2 = 0U;
  }
  else if (shift == 192){
    data3 = data0;
    data0 = 0;
    data1 = 0;
    data2 = 0;
  }
  else if (shift > 128){
    //a bits got preserved
    int a = 256 - shift;
    //first few bits in data1: we need a-64 bits of data1
    int e = a - 64;
    uint64_t temp1 = data1 & ~(~0U << e);
    //then we need b=64-e bits to fill data3, i.e, the leading b bits of data0
    int b = 64 - e;
    
    //then take the rest bits of data0 out
    int c = 64 - b;
    uint64_t temp2 = data0 >> c;
    //the first c bits of data0 will go to 
    uint64_t temp3 = data0 & ~(~0U << c);
    //the let's plug these values to data3 first
    data3 = (temp1 << b) + temp2;
    data2 = temp3 << (64-c);
    data1 = 0U;
    data0 = 0U;   
  }
  else if (shift == 128){
    data3 = data1;
    data2 = data0;
    data1 = 0;
    data0 = 0;
  }
  else if(shift > 64){
    //we need to eleminate leading shift-64 bit of data 2 other than whole data3 
    int a = shift - 64;
    int b = 64 - a; // b bits of data2 get preserved;
    int c = 64 - b; // c more bits of data1 needed to fill data3
    int d = 64 - c; // d bits to right shift
    uint64_t temp1 = data2 & ~(~0U << b);
    uint64_t temp2 = data1 >> d;
    data3 = (temp1 << c) + temp2;
    uint64_t temp3 = data1 & ~(~0U << d); // this gives first d bits of data1
    int e = 64 - d; //we need e more bits of data0 to fill data2
    int f = 64 - e; //f bits to right shift
    uint64_t temp4 = data0 >> f;
    data2 = (temp3 << e) + temp4;
    int g = 64 - f; //we have g bits of data0 left
    uint64_t temp5 = data0 & ~(~0U << g);
    
    data1 = temp5 << g;
    data0 = 0;
    
  }
  else if(shift == 64){
    data3 = data2;
    data2 = data1;
    data1 = data0;
    data0 = 0;
  }
  else{
    int a = 64 - shift; //we preserve first a bits of data3
    int b = 64 - a; // b leading bits for data2 required to fill data3
    int c = 64 - b; // c bits need to left shift
    uint64_t temp1 = data3 & ~(~0U << a);
    uint64_t temp2 = data2 >> c;
    data3 = (temp1 << b) + temp2;
    uint64_t temp3 = data2 & ~(~0U << c);
    int d = 64 - c; // d more bits of data1 needed to fill data2
    int e = 64 - d; // e bits right shift
    uint64_t temp4 = data1 >> e;
    data2 = (temp3 << d) + temp4;
    int f = 64 - e; //f bits of data0 needed to fill data1
    uint64_t temp5 = data1 & ~(~0U << e);
    int g = 64 - f; //g bits to right shift
    uint64_t temp6 = data0 >> g;
    data1 = (temp5 << f) + temp6;
    data0 = data0 << (64-g);
  }
  uint64_t arr[4] = {data0, data1, data2, data3};
  result = uint256_create(arr);
  return result;
}


// Compute the product of two UInt256 values.
UInt256 uint256_mul(UInt256 left, UInt256 right) {
  UInt256 product;
  // TODO: implement
  return product;
}

int uint256_bit_is_set(UInt256 val, unsigned index) {
  if(index / 64 == 0){
    if(val.data[0] & (1 << (index % 64))){
      return 1;
    }
    return 0;

  } else if(index / 64 == 1){
      if(val.data[1] & (1 << (index % 64))){
        return 1;
      }
      return 0;
  
  } else if (index / 64 == 2){
      if(val.data[2] & (1 << (index % 64))){
        return 1;
      }
      return 0;
  } else if (index / 64 == 3){
      if(val.data[3] & (1 << (index % 64))){
        return 1;
      }
      return 0;
  }
}
