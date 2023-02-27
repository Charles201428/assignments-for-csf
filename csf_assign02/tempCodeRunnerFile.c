void printsupplement(int index) {
  if(index % 16) { //this shows how many hex dig are on this line
    int i = 0;
    while(i < 16 - (index % 16)) {
      hex_write_string("   ");
      i++; 
    }
    //space generated until allignment
  }
}