#include "cache.h"
#include <iostream>

#define ADDR_LEN 32

// Assume x >=0
uint32_t get_least_sig_n(uint32_t num, int n) {
    return num & ~(~0U << n);
}

//get lowest m bits if ignoring last n bits
uint32_t get_low_sig(uint32_t num, int n, int m) {
    uint32_t temp = num >> n;
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

// Constructor
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
        index_len = highest_bitSet(setNum);
        offset_len = highest_bitSet(byteNum);
        tag_len = 32 - index_len - offset_len;
        
    }


bool cache::is_set_full(uint32_t index){
    //find the set at given index
    Set at_index = this->data[index];
    //test whether the size of vector of sets is >= index
    bool whether = at_index.size() == blockNum;
    return whether;
}


bool cache::is_hit(uint32_t index, uint32_t tag) {
    //find the set at given index
    
    Set at_index = data[index];
    Set::iterator it = at_index.find(tag);
    return it != at_index.end(); 
    //return (data[index].find(tag) != data[index].end());

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
		// delete the block that should be evicted
		uint32_t key;
		for (Set::iterator it = at_index.begin();
			it != at_index.end();
			it++) {
			if (it->second.first == (blockNum-1)) {
				if (it->second.second == true) {
					cycles += byteNum * 25; 
				}
				key = it->first;
            }
        }
		at_index.erase(key);
	}
	// increase the counter 
	for (Set::iterator it = at_index.begin();
		it != at_index.end();
		it++) {
		(it->second.first)++;
	}
	// add new block
	at_index[tag].first = 0;
	at_index[tag].second = false;
    data[index] = at_index;

    loadMiss++;
	cycles += byteNum * 25;
}


void cache::load(uint32_t address) {
    uint32_t index = get_low_sig(address, offset_len, index_len);
    uint32_t tag = get_low_sig(address, index_len + offset_len, tag_len);
    if (is_hit(index, tag)) {
        count_load_hit(index, tag);
    }
    else {
        count_load_miss(index, tag);
    }
    totalLoad++;

}



void cache::count_store_hit(uint32_t index, uint32_t tag) {
	(this->storeHit)++;
	cycles++;
	//if (evict_type == LRU) {
		// need to renew the counter when evict type is LRU
		uint32_t order = (data[index])[tag].first; // get timestamp
		for (Set::iterator it = data[index].begin();
			it != data[index].end();
			it++) {
			// it->second denote the (tag, Block) pair for all blocks in the set
			// it->second.first is the timestamp. Increment it for all elements whose timestamp
			// was smaller than the original timestamp of the block that is hit
			if (it->second.first < order) {
				it->second.first++;
			}
		}
		(data[index])[tag].first = 0;
	//}
	// When it is write through, we also need to to write to main memory
	if (write_through) {
		cycles = cycles + 100;
	} else {
		// mark as dirty when it is write back type
		(data[index])[tag].second = true;
	}
}



void cache::count_store_miss(uint32_t index, uint32_t tag) {
	(this->storeMiss)++;
	if (write_allocate) {
		// we need to change the set when it is write_allocate
		cycles = cycles + 25 * byteNum;
		if (is_set_full(index)) {
			// delete the block that should be evicted
			uint32_t to_delete;
			for (Set::iterator it = data[index].begin();
				it != data[index].end();
				it++) {
				if (it->second.first == (blockNum-1)) {
					if (it->second.second == true) {
						cycles=cycles + byteNum * 25; 
					}
					to_delete = it->first;
				} 
			}
			data[index].erase(to_delete);
		}
		// increase the counter
		for (Set::iterator it = data[index].begin();
			it != data[index].end();
			it++) {
			(it->second.first)++;
		}
		// add new element
		(data[index])[tag].first = 0;
		(data[index])[tag].second = true;
		cycles++;
		if (write_through) {
			// write to memory
			(data[index])[tag].second = false;
			cycles += 100;
		}
	} else {
		// when it is no-write-allocate, write to memory and cache is unchanged
		cycles = cycles + 100;
	}
}

void cache::store(uint32_t address) { //similar to load
    uint32_t index = get_low_sig(address, offset_len, index_len);
    uint32_t tag = get_low_sig(address, offset_len + index_len, tag_len);
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
    std::cout << "Total cycles: " << cycles << std::endl;
}