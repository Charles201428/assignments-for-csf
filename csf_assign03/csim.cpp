#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <map>
#include <iostream>

using std::string;
using std::cin;
using std::cerr;

//this fuc sets the 
int set_write_allocate_mode(string input){
    string write = "write-allocate";
    string noWrite = "no-write-allocate";

    if (write == input) return 0;
    else if (noWrite == input) return 1;
    return -1; 
}

int main(){
    return 0;
}