#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <map>
#include <iostream>
#include <cmath>


/*consider a block, it only contains two useful info, i.e, counter (this counter is order in the case of LRU) and dirtybit
* LRU: counter stands for the number of access described in the hw page
*FIFO: counter stands for order as described in the hw page
* Write-back: whether dirty byte or not
* note: we consider first entry as LRU order by default for ms2
*/
typedef std::pair<uint64_t, bool> Block;


//a settype is a map from key (tag) to value (block)
typedef std::unordered_map<uint32_t, Block> Set;
//then the cache type is a vector of SetType where each entry stands for the type of corresponding set
typedef std::vector<Set> CacheType;



class cache {

public:
    // constructor
    cache(
        int setNum, 
        int blockNum, 
        int byteNume, 
        bool write_allocate, 
        bool write_through
        //bool LRU,
        //bool FIFO, not gonna take care of this case for ms2
        //variable names are intutive
    );

    // Destructor
    ~cache(){};

    
    void load(uint32_t address);

    
    void store(uint32_t address);

    // print output
    void output();

private:
    // internal variables used to keep track of the state of the cache. Names are self-explanatory
    int setNum, blockNum, byteNum;
    bool write_allocate, write_through; //LRU, FIFO (for ms3)
    int indexLen, tagLen, offsetLen; // 2^(indexlen) + setnums + bytenum = addressLen
    int totalLoad, totalStore, loadHit, loadMiss, storeHit, storeMiss, cycles;
    int index_len, offset_len, tag_len;

    // the main data structure to store what do we have in the cache
    CacheType data;

    // check if |tag| is in the set indicated by |index|
    bool is_hit(uint32_t index, uint32_t tag);

    // check if the set indicated by |index| is full
    bool is_set_full(uint32_t index);

    //following few functions update counts/order in the scenarios as described in the function name
    void count_load_hit(uint32_t index, uint32_t tag);
    void count_load_miss(uint32_t index, uint32_t tag);
    void count_store_hit(uint32_t index, uint32_t tag);
    void count_store_miss(uint32_t index, uint32_t tag);    

};





#endif