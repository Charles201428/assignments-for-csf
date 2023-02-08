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
  result.data[0] = 0;
  result.data[1] = 0;
  result.data[2] = 0;
  result.data[3] = 0;
  int len = strlen(hex);
  //store the rightmost 64 digits of the hex if the length of hex exceeds 64
  if (len >= 64){
    char newstuff[65];
    for (int i = 0; i < 64; i++){
      newstuff[i] = hex[i+len-64];
    }
    newstuff[64] = '\0';
    char newchar1[17];
    char newchar2[17];
    char newchar3[17];
    char newchar4[17];
    //only store the first 64 chars and divide them into group per 16 chars
    for (int i = 0; i < 16; i++){
      newchar1[i] = newstuff[i];
    }
    newchar1[16] = '\0';
    for (int j = 16; j < 32; j++){
      newchar2[j-16] = newstuff[j];
    }
    newchar2[16] = '\0';
    for (int k = 32; k < 48; k++){
      newchar3[k-32] = newstuff[k];
    }
    newchar3[16] = '\0';
    for (int l = 48; l < 64; l++){
      newchar4[l-48] = newstuff[l];
    }
    newchar4[16] = '\0';
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
    int quo = len/16;
    int rem = len%16;
    /*uint64_t data0 = 0;
    uint64_t data1 = 0;
    uint64_t data2 = 0;
    uint64_t data3 = 0;
    */
    char remain[rem+1];
    for (int j = 0; j < rem; j++){
      remain[j] = hex[j];
    }
    remain[rem] = '\0';
    char *strsig;
    uint64_t significant = strtoul(remain, &strsig, 16);
    result.data[quo] = significant;
    printf("current quo: %lx. current num: %lx\n", quo, result.data[quo]);
    char rest[len-rem+1];
    
    for (int i = 0; i < len-rem; i++){
      rest[i] = hex[i+rem];      
    }
    rest[len-rem] = '\0';
    //int newlen = strlen(rest);
    for (int i = 0; i < quo; i++){
      char *stri;
      char quoi[17];
      quoi[16] = '\0';
      for (int j = 0; j < 16; j++){
        quoi[j] = rest[i*16+j];
      }
      uint64_t inti = strtoul(quoi, &stri, 16);
      result.data[quo-i-1] = inti;
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
    
    data0 = 0UL;
    data1 = 0UL;
    data2 = 0UL;
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
    //first b bits of data1 get preserved;
    int b = a - 64;
    // c bits to left shift
    int c = 64 - b;
    data3 = (data1 << c) + (data0 >> b);
    data2 = data0 << c;
    data1 = 0;
    data0 = 0;
  }
  else if (shift == 128){
    data3 = data1;
    data2 = data0;
    data1 = 0;
    data0 = 0;
  }
  else if(shift > 64){
    int comp1 = shift - 64;
    int comp2 = 64 - comp1;
    data3 = (data2 << comp1) + (data1 >> comp2);
    data2 = (data1 << comp1) + (data0 >> comp2);
    data1 = (data0 << comp1); 
    data0 = 0;
    
  }
  else if(shift == 64){
    data3 = data2;
    data2 = data1;
    data1 = data0;
    data0 = 0;
  }
  else if(shift > 0){
    int comp = 64 - shift;
    data3 = (data3 << shift) + (data2 >> comp);
    data2 = (data2 << shift) + (data1 >> comp);
    data1 = (data1 << shift) + (data0 >> comp);
    data0 = (data0 << shift);
  }
  uint64_t arr[4] = {data0, data1, data2, data3};
  result = uint256_create(arr);
  return result;
}

int uint256_bit_is_set(UInt256 val, unsigned index) {
  if(index / 64 == 0){
    if(val.data[0] & (1UL << (index % 64))){
      return 1;
    }
    return 0;

  } else if(index / 64 == 1){
      if(val.data[1] & (1UL << (index % 64))){
        return 1;
      }
      return 0;
  
  } else if (index / 64 == 2){
      if(val.data[2] & (1UL << (index % 64))){
        return 1;
      }
      return 0;
  } else if (index / 64 == 3){
      if(val.data[3] & (1UL << (index % 64))){
        return 1;
      }
      return 0;
  }
  return 0;
}

UInt256 uint256_mul(UInt256 left, UInt256 right) {
  UInt256 product;
  product.data[0] = 0;
  product.data[1] = 0;
  product.data[2] = 0;
  product.data[3] = 0;
  
  for (unsigned int i = 0; i < 256; i++){
    if(uint256_bit_is_set(left, i)){
     UInt256 res = uint256_leftshift(right, i);
     product = uint256_add(product, res);
    }

  }

  return product;
}


