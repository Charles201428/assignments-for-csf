// C implementation of hexdump main function

#include "hexfuncs.h"  // this is the only header file which may be included!
//we need to print offset, one-space, hex-version of each character, 2-space, string-rep

//print offset
void printoffset(int value, char offset[]) {
  hex_format_offset(value, offset);
  hex_write_string(offset);
  hex_write_string(": "); 
  //note that this also includes the space wanted 
}

//this is prepared to print the converted printable 
void printdata(char data[]) {
  hex_write_string(" ");
  hex_write_string(data);
  hex_write_string("\n"); 
  
}

// this function prints the space between hax pair and printables to reach allignment
void printsuplement(int string_index) {
  if(string_index % 16) { //this shows how many hex dig are on this line
    int i = 0;
    while(i < 16 - (string_index % 16)) {
      hex_write_string("   ");
      i++; 
    }
    //space generated until allignment
  }
}



int main(void) {
  //this offset stores the 8 digit hex
  char offset[9];
  // this data stores the data read from std input
  char data[17];
  // manifesting how many pairs of hex appear at the current line
  long index = 0;
  // store the temporary pair of hex for each character in the input string
  char hex[3];
  //read from std input
  int readlength = hex_read(data);

  while (readlength > 0) { //update readlength at each loop by calling hex_read
    printoffset(index, offset);
    for(int i = 0; i < readlength; i++) {
      //loop through every element of this string
      hex_format_byte_as_hex(data[i], hex); 
      hex_write_string(hex); 
      // then we print out the hex pair
      hex_write_string(" "); 
      data[i] = hex_to_printable(data[i]); //conversion to printable
      index += 1; 
    }
    printsupplement(index);// use space to suplemlent until allignment 
    printdata(data);
    //current line finished, read in data from std input to next line
    readlength = hex_read(data); 
  }
  return 0;
}
