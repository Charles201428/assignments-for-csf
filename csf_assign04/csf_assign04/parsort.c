#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}


// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  // TODO: implement
  size_t prob_size = end - begin + 1;
  if (prob_size <= threshold) {
    qsort(arr + begin, prob_size, sizeof(int64_t), cmpfunc);
  }
  else {
    pid_t leftkid, rightkid;
    leftkid = fork();
    if (leftkid == -1) {
      fprintf(stderr, "%s", "Error: failure to create a child process using fork.\n");
      exit(-1);
    }
    else if (leftkid == 0) {
      
    }
  }
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2]))
    /* TODO: report an error (threshold value is invalid) */;

  // TODO: open the file

  // TODO: use fstat to determine the size of the file

  // TODO: map the file into memory using mmap

  // TODO: sort the data!

  // TODO: unmap and close the file

  // TODO: exit with a 0 exit code if sort was successful
}
