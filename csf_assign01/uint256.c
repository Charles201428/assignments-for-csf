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
  char *hex = NULL;
  // TODO: implement
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
  // TODO: implement
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

// Compute the product of two UInt256 values.
UInt256 uint256_mul(UInt256 left, UInt256 right) {
  UInt256 product;
  // TODO: implement
  return product;
}
