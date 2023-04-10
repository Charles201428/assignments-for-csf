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



//cmp function required for qsort
int compare_i64 (const void * a, const void * b) {
   return ( *(int64_t*)a - *(int64_t*)b );
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
  size_t prob_size = end - begin;
  size_t mid = (end + begin) / 2;
  pid_t leftkid, rightkid;
  if (prob_size <= threshold) {
    qsort(arr + begin, prob_size, sizeof(int64_t), compare_i64);
  }
  
  leftkid = fork(); //kid process for left kid
  if (leftkid == -1) {
    perror("Error: failure to create a child process using fork.\n");
    exit(-1);
  }
  else if (leftkid == 0) {
    merge_sort(arr, begin, mid, threshold);
    exit(0);
  }
  
  rightkid = fork(); //right kid
  if (rightkid == -1) {
    perror("Error: failure to creat a child process using fork.\n");
    exit(1);
  }
  else if (rightkid == 0) {
    merge_sort(arr, begin + prob_size/2, end, threshold);
    exit(0);
  }


  int left_wstatus, right_wstatus;
  pid_t left_actual, right_actual;
  left_actual = waitpid(leftkid, &left_wstatus, 0);
  right_actual = waitpid(rightkid, &right_wstatus, 0);
  if (left_actual == -1) {
    perror("Error: waitpid left kid fails.\n");
    exit(-2);
  }
  else if (!WIFEXITED(left_wstatus)) {
    perror("left kid subprocess crashed, was interrupted, or did not exit normally\n");
    exit(-3);
  }
  else if (WIFEXITED(left_wstatus) != 0) {
    perror("left kid subprocess returned a non-zero exit code.\n");
    exit(-4);
  }
  if (right_actual == -1) {
    perror("Error: waitpid right kid fails.\n");
    exit(2);
  }
  else if (!WIFEXITED(right_wstatus)) {
    perror("right kid subprocess crashed, was interrupted, or did not exit normally\n");
    exit(3);
  }
  else if (WIFEXITED(right_wstatus) != 0) {
    perror("right kid subprocess returned a non-zero exit code.\n");
    exit(4);
  }
  
  //merging part
  int64_t* temp = (int64_t *) malloc(prob_size * sizeof(int64_t));
  merge(arr, begin, mid, end, temp);
  for (int i = 0; i < prob_size; i++) {
    arr[i] = temp[i];
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
  if (end != argv[2] + strlen(argv[2])){
    fprintf(stderr, "Error: the threshold value is invalid.\n");
    return 2;
  }
  // TODO: open the file

  // TODO: use fstat to determine the size of the file
    int fd = open(filename, O_RDWR);
    if(fd == -1) {
        perror("Error opening file");
        return 1;
    }
    struct stat sb;
    if(fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        return 1;
    }
    size_t file_size = sb.st_size;
    size_t length = file_size / sizeof(int64_t);

  // TODO: map the file into memory using mmap
    int64_t *mapped = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("Error mapping file");
        return 1;
    }

  // Now the file is mapped into memory, and you can access it using the 'mapped' pointer.
  // TODO: sort the data!
    merge_sort(mapped, 0, length, threshold);

  // TODO: unmap and close the file

  // TODO: exit with a 0 exit code if sort was successful

    if (munmap(mapped, file_size) == -1) {
        perror("Error unmapping file");
        return 3;
    }

    if (close(fd) == -1) {
        perror("Error closing file");
        return 3;
    }

    return 0;
}
