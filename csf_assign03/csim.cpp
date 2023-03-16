#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include "cache.h"
#include "cache.cpp"

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;







bool is_power_of_two(int n) {
    int temp = n & (n-1);
    return temp == 0;
}

int main(int argc, char *argv[]) {
    //main goal is to handle errors/exceptions/invalid inputs here i.e, from command line argument
    
    int setNum, blockNum, byteNum;
    bool wirte_allocate, write_through;

    if (argc != 7) {
        cerr << "Invalid command line argument!" << endl;
        return 1;
    }

    try
    { //test whether these three arg are all int
        setNum = std::stoi(argv[1]);
        blockNum = std::stoi(argv[2]);
        byteNum = std::stoi(argv[3]);
    }
    catch(const std::exception& e)
    {
        cerr << "Invalid command line argument!" << endl;
        return 1;
    }
    
    if (strcmp(argv[4], "write-allocate") == 0 || (strcmp(argv[4], "no-write-allocate") == 0)) {
        wirte_allocate = strcmp(argv[4], "write-allocate") == 0;
    }
    else {
        cerr << "Invalid command line argument!" << endl;
        return 1;
    }

    if (strcmp(argv[5], "write-through") == 0 || (strcmp(argv[5], "write-back") == 0)) {
        write_through = strcmp(argv[5], "write-through") == 0;
    }
    else {
        cerr << "Invalid command line argument!" << endl;
        return 1;
    }

     if (strcmp(argv[6], "lru") != 0 && (strcmp(argv[6], "fifo") != 0)) {
        cerr << "Invalid command line argument!" << endl;
        return 1;
    }
    
    if (!(is_power_of_two(setNum)) || !(is_power_of_two(blockNum)) || !(is_power_of_two(blockNum)) || byteNum < 4) {
        cerr << "Invalid command line argument!" << endl;
        return 1;
    }

    cache the_cache(setNum, blockNum, byteNum, wirte_allocate, write_through);
    string LorS, address, c;
    while (cin >> LorS >> address >> c) {
        uint32_t addressnum = stoi(address);
        if (LorS == "l") {
            the_cache.load(addressnum);
        }
        else if (LorS == "s") {
            the_cache.store(addressnum);
        }

    }
    the_cache.output();

    return 0;
}
