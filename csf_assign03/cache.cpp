#include "cache.h"
#include <iostream>

#define ADDR_LEN 32

// Assume x >=0
int highest_bit_set(int x) {
    int counter = -1;
    while (x) {
        x >>= 1;
	counter++;
    }
    return counter;
}

// from low to high, ignore first a bits and get the following b bits
uint32_t get_bits(uint32_t x, int a, int b) {
    uint32_t mask = ~(~0L << b);
    return (mask & (x >> a)); 
}

// Constructor
cache::cache(
        int setNum, 
        int blockNum, 
        int byteNum, 
        bool write_allocate, // true: write-allocate | false: no-write-allocate 
        bool write_through // true: write-through | false: write-back
        //bool evict_type // true: lru | false: fifo
    ): 
        setNum(setNum), blockNum(blockNum), byteNum(byteNum), 
        write_allocate(write_allocate), write_through(write_through), //evict_type(evict_type),
        totalLoad(0), loadHit(0), loadMiss(0), totalStore(0), storeHit(0), storeMiss(0), cycles(0),
        data(setNum, Set()) // need to initialize every map object inside the vector!
    {
        // we have already checked num_sets and num_bytes are power of 2
        this->index_len = highest_bit_set(setNum);
        this->offset_len = highest_bit_set(byteNum);
        this->tag_len = ADDR_LEN - this->index_len - this->offset_len; // #tag = ADDR_LEN - #index - #offset
    }



bool cache::is_hit(uint32_t index, uint32_t tag) {
	// if we can find |tag| in the keys of data[index], that means tag is in the set
    return (data[index].find(tag) != data[index].end());
}

bool cache::is_set_full(uint32_t index) {
	// number of blocks in a set can't exceed |num_blocks|
  	return (data[index].size() == blockNum);
}

void cache::load(uint32_t address) {
	(this->totalLoad)++;
	uint32_t index = get_bits(address, this->offset_len, this->index_len);
	uint32_t tag = get_bits(address, this->offset_len+this->index_len, this->tag_len);
	// check whether miss or not
	if (is_hit(index, tag)) {
		count_load_hit(index, tag);
	} else {
		count_load_miss(index, tag);
	}
}

void cache::count_load_hit(uint32_t index, uint32_t tag) {
	(this->loadHit)++; 
	cycles++;
	//if (evict_type == LRU) {
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
}

void cache::count_load_miss(uint32_t index, uint32_t tag) {
	(this->loadMiss)++;
	cycles=cycles + byteNum * 25;
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
	// add new block
	(data[index])[tag].first = 0;
	(data[index])[tag].second = false;
}



void cache::store(uint32_t address) {
	(this->totalStore)++;
	uint32_t index = get_bits(address, this->offset_len, this->index_len);
	uint32_t tag = get_bits(address, this->offset_len+this->index_len, this->tag_len);
	// check whether miss or not
	if (is_hit(index, tag)) {
		count_store_hit(index, tag);
	} else {
		count_store_miss(index, tag);
	}
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

/*
 * Format:
 * Total loads: count
 * Total stores: count
 * Load hits: count
 * Load misses: count
 * Store hits: count
 * Store misses: count
 * Total cycles: count
 */
void cache::output() {
    std::cout << "Total loads: " << this->totalLoad << std::endl;
    std::cout << "Total stores: " << this->totalStore << std::endl;
    std::cout << "Load hits: " << this->loadHit << std::endl;
    std::cout << "Load misses: " << this->loadMiss << std::endl;
    std::cout << "Store hits: " << this->storeHit << std::endl;
    std::cout << "Store misses: " << this->storeMiss << std::endl;
    std::cout << "Total cycles: " << this->cycles << std::endl;
}
