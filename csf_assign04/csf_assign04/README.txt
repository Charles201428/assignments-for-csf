CONTRIBUTIONS

Charles is responsible for the Merge_sort. Chengqian is responsible for the
main and debugging. 

REPORT

Experiments data: 

real    0m0.639s
user    0m0.285s
sys     0m0.029s

real    0m0.573s
user    0m0.348s
sys     0m0.047s

real    0m0.545s
user    0m0.369s
sys     0m0.058s

real    0m0.499s
user    0m0.470s
sys     0m0.059s

real    0m0.296s
user    0m0.589s
sys     0m0.078s

real    0m0.228s
user    0m0.591s
sys     0m0.085s

real    0m0.221s
user    0m0.653s
sys     0m0.112s

real    0m0.225s
user    0m0.981s
sys     0m0.148s

The time is gradually descreasing for each experiement above.

1. As we can see the real time is descreasing until certain point and then the time starts to rise again.
This is very reasonable due to the following reasons. 

2. More and more child process are running at the same time.
This will clearly save the total time of running. When these child processes are 
running in parallel, it will save the total amount of time needed. The qsort() can
run parallel is because there is no dependency among them. However, because running the
program parallelly requires the GPU cores. If we gradually require more and more GPU cores, finally
we will run out of GPU cores, so in this case, the effiency won't be significantly improved; instead,
it may be negatively impacted. The child process - merge sort part - can be shceduled by the OS kernel 
in parallel on different CPU cores.
