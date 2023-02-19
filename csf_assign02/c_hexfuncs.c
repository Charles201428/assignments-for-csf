// C implementation of hexdump functions

#include <unistd.h>  // this is the only system header file you may include!
#include "hexfuncs.h"

// TODO: add function implementations here
unsigned hex_read(char data_buf[]){
    int len = 0;
    len = read(0, data_buf, 16);
    data_buf[len] = '\0';
    return len;
  }



void hex_write_string(const char s[]){
    //since we can't use strlen, we need to loop each character until the end of the string
    int count = 0;
    while (s[count] != '\0'){
      write(1, s+count, 1);
      count++;
    }

}

void hex_format_offset(unsigned offset, char sbuf[]){
    for (int i = 0; i < 8; i++){
      //unsigned has 32 bit, and 4 bits should be stored as a hex-digit per time
      int curdigits = 0;
      char hex = '0';
      //get the wanted  four bit;
      curdigits = offset>>(4*i) & ~(~0U << 4);
      //0-9 just get mapped to "0" - "9"
      if (curdigits < 10){
        hex = curdigits + 48;

      }
      else{
        //10-15 get maps to a-f
        hex = curdigits + 87;
      }
      sbuf[7-i] = hex;

    }
    sbuf[8] = '\0';
}


void hex_format_byte_as_hex(unsigned char byteval, char sbuf[]){
   
  //pretty similar to format_offset if we realize that a char is itself an int
  for (int i = 0; i < 2; i++){
    int curdigits = 0;
    char hex = '0';
    curdigits = byteval>>(4*i) & ~(~0U << 4);
    if (curdigits < 10){
      hex = curdigits + 48;
    }
    else{
      hex = curdigits +87; 
    }
    sbuf[1-i] = hex;
  }
  sbuf[2] = '\0';
}

char hex_to_printable(unsigned char byteval){
  //note that anything whose ascii value is smaller 32 or greater than 126 is not really printable
  if (byteval < 32 || byteval > 126){
    char newby = '.';
    return newby;
  }
  return byteval;
}