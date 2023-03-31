Charles Weng (yweng13)
Chengqian Fu (cfu19)
Charles did cache class construction Debugging
Chengqian did main file/error handling ReadMe and Debugging

1. Between FIFO/LRU (Independent parameter):

./csim 256 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 9841257

./csim 256 4 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9341084

./csim 256 4 16 write-allocate write-through fifo < gcc.trace

Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 25646257

./csim 256 4 16 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 25314884

LRU has better performance, since clearly it has higher hiting rate lower missing rate,
and the total cycles are also less than the FIFO

2. Between: Write through/write back (Independent parameter):

./csim 256 4 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9341084

./csim 256 4 16 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 25314884

write back is clearly better based on the data shown above. The logic also makes sense since writing back
won't write back to memory many times and this will save lots of time.


3. Block Size (Independent parameter):

./csim 256 8 8 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 313484
Load misses: 4713
Store hits: 179709
Store misses: 17777
Total cycles: 8488970

./csim 256 16 4 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 312729
Load misses: 5468
Store hits: 169711
Store misses: 27775
Total cycles: 6423915

./csim 256 8 8 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 312729
Load misses: 5468
Store hits: 179369
Store misses: 18117
Total cycles: 8828215

./csim 256 16 4 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 312031
Load misses: 6166
Store hits: 169388
Store misses: 28098
Total cycles: 6581417

We can see a balance here. When the block size (8) is smaller, it has more cycles but
also higher hitting rate/lower missing rate.


4.Write-allocator and no-writer-allocator:

./csim 256 4 16 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 25646257


./csim 256 4 16 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 311017
Load misses: 7180
Store hits: 163705
Store misses: 33781
Total cycles: 23095322


./csim 256 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 9841257

We can find that write-allocate has higher load hits and fewer hit misses. In general,
after comparing these three examples above, we can say that write allocate is better.


5.Direct Mapping, Set associative, Fully associative (Independent parameter):

Direct Mapping:
./csim 256 1 16 write-allocate write-back lru < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 298863
Load misses: 19334
Store hits: 185202
Store misses: 12284
Total cycles: 20293149

Set associative:
./csim 256 4 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9341084

Fully associative:
./csim 1 4 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 120364
Load misses: 197833
Store hits: 101401
Store misses: 96085
Total cycles: 165355050

Set-associative is the best, both in terms of the cycles and missing/hitting rate

The final conclusion. The below is the best configuration is using the write-back, LRU, and write-allocate set-associative; the size of the cache is
more like a trade-off.

Reasons behind the conclusion:
The set-associative is great because it strikes balance between direct mapping and fully associative. For the write through, it's not that
efficient because it needs to keep updating to the memory, and updating to the memory is much slower than updating to the cache.
LRU also works really efficient because it can identify the not frequently used blocks. This is crucial since some blocks may not be the first to be added in but haven't
been used for a long time, while some other blocks are added to the cache very early but are still frequently used.
