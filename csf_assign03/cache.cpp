#include "cache.h"
#include <iostream>

//the least significant n bits
uint32_t get_least_sig_n(uint32_t num, int n) {
    return num & ~(~0U << n);
}

//get lowest m bits if ignoring last n bits
uint32_t get_low_sig(uint32_t num, int n, int m) {
    uint32_t temp = get_least_sig_n(num, n);
    return get_least_sig_n(temp, m);
}

//fnd how many bit an uint has
int highest_bitSet(uint num) {
    int highest = 0;
    while (num >>= 1) {
        highest++;
    }
    return highest;
}

//then implement the class funcs

cache::cache(
        int setNum, 
        int blockNum, 
        int byteNum, 
        bool write_allocate, 
        bool write_through
    ) {
        this->setNum = setNum;
        this->byteNum = byteNum;
        this->blockNum = blockNum;
        this->write_allocate = write_allocate;
        this->write_through = write_through;
        totalLoad = 0;
        totalStore = 0;
        loadHit = 0;
        loadMiss = 0;
        storeHit = 0;
        storeMiss = 0;
        cycles = 0;
        data = CacheType(setNum, Set());
        
    }

bool cache::is_set_full(uint32_t index){
    //find the set at given index
    Set at_index = this->data[index];
    //test whether the size of vector of sets is >= index
    bool whether = at_index.size() >= blockNum;
    return whether;
}


bool cache::is_hit(uint32_t index, uint32_t tag) {
    //find the set at given index
    /*
    Set at_index = data[index];
    Set::iterator it = at_index.find(tag);
    return it != at_index.end(); */
    return (data[index].find(tag) != data[index].end());

}

void cache::count_load_hit(uint32_t index, uint32_t tag){
    //any access will triger lru order change
    //find the set at given index
    Set at_index = this->data[index];
    //determine which counter we need to increment
    int maxi = at_index[tag].first;
    for (Set::iterator it = at_index.begin(); it != at_index.end(); it++) {
        if(it->second.first < maxi) {
            it->second.first++;
        }
    }
    //set tag block to be 0-order
    at_index[tag].first = 0;
    data[index] = at_index;
    //increment stuff
    loadHit++;
    //note that from memory trasfering each byte's consumption is assumed to be 100/4 = 25
    cycles ++;


}

void cache::count_load_miss(uint32_t index, uint32_t tag) {
    Set at_index = data[index];
    if (is_set_full(index)) {
        uint32_t key = (at_index.begin()) -> first;
        uint32_t maxi = (at_index.begin()) -> second.first;
        for (Set::iterator it = at_index.begin(); it != at_index.end(); it++) {
            if (maxi < it->second.first) {
                maxi = it->second.first;
                key = it->first;
            }
        }
        Block temp = at_index[key];
        if (temp.second == true) {
            cycles += byteNum * 25;
        }
        at_index.erase(key);
    }
    else {
        for (Set::iterator it = at_index.begin(); it != at_index.end(); it++) {
            it->second.first++;
        }
    }
    Block newBlock;
    newBlock.first = 0;
    newBlock.second = false;
    at_index[tag] = newBlock;
    data[index] = at_index;
    loadMiss++;
    cycles += byteNum * 25;
}


void cache::load(uint32_t address) {
    uint32_t index = get_least_sig_n(address, highest_bitSet(setNum));
    uint32_t tag = get_low_sig(address, highest_bitSet(setNum), highest_bitSet(blockNum));
    if (is_hit(index, tag)) {
        count_load_hit(index, tag);
    }
    else {
        count_load_miss(index, tag);
    }
    totalLoad++;

}


void cache::count_store_hit(uint32_t index, uint32_t tag) {
    Set at_index = this->data[index];
    //update order
    //similar to the one in load_hit
    int maxi = at_index[tag].first;
    for (Set::iterator it = at_index.begin(); it != at_index.end(); it++) {
        if(it->second.first < maxi) {
            it->second.first++;
        }
    }
    //set tag block to be 0-order
    at_index[tag].first = 0;
    //no store miss, so write-allocate or not doesn't matter
    if (write_through) {
        cycles += byteNum * 25;
    }
    else {
        at_index[tag].second = true;
    }
    data[index] = at_index;
    //counters
    storeHit++;
}

void cache::count_store_miss(uint32_t index, uint32_t tag) {
    Set at_index = Set();
    if (write_allocate) {
        Set::iterator it = at_index.begin();
        if(is_set_full(index)) {
            //need to evict one with largest order
            Block max = it->second;
            uint32_t key = it->first;
            //find the one with largest order
            for (it; it != at_index.end(); it++) {
                if (it->second.first < max.first) {
                    max = it->second;
                    key = it->first;
                }
                //and smartly update the order
                it->second.first++; 
            }
            if (max.second == true) { //dirty
                cycles += byteNum * 25;
            }
            at_index.erase(key);
        }
        else { //only update order in this case
            for (it; it != at_index.end(); it++) {
                it->second.first++;
            }
        }
        //insert new block
        Block newBlock;
        newBlock.first = 0;
        newBlock.second = false;
        at_index[tag] = newBlock;
        //access the cache anyway
        cycles++;
        data[index] = at_index;
    }
    else {
        cycles += byteNum * 25;
    }
    storeMiss++; //anyway increment this
}



void cache::store(uint32_t address) { //similar to load
    uint32_t index = get_least_sig_n(address, highest_bitSet(setNum));
    uint32_t tag = get_low_sig(address, highest_bitSet(setNum), highest_bitSet(blockNum));
    if (is_hit(index, tag)) {
        count_store_hit(index, tag);
    }
    else {
        count_store_miss(index, tag);
    }
    totalStore++;
}

void cache::output() {
    std::cout << "Total loads: " << totalLoad << std::endl;
    std::cout << "Total stores: " << totalStore << std::endl;
    std::cout << "Load hits: " << loadHit << std::endl;
    std::cout << "Load misses: " << loadMiss << std::endl;
    std::cout << "Store hits: " << storeHit << std::endl;
    std::cout << "Store misses: " << storeMiss << std::endl;
    std::cout << "Total cycles: " << this->cycles << std::endl;
}