/*
 * C implementation of merge sort utilizing parallelism
 * CSF Assignment 4 
 * Bill Chen, Kit Ao
 * jchen363@jh.edu, mao2@jh.edu
 */

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
#include <stdarg.h>

/**
 *  A funtion that compares two elements which is used for qsort
 *  
 *  Parameters:
 *    left - a pointer pointing to the left element
 *    right - a pointer pointing to the right element
 */
int compare_int64(const void* left, const void* right) {
  int64_t left_num = *(const int64_t *) left;
  int64_t right_num = *(const int64_t *) right;

  if (left_num < right_num) {
    return -1;
  } else if (left_num > right_num) {
    return 1;
  }
  return 0;
}

/**
 *  Merges the sorted left half with the right half
 *  
 *  Parameters:
 *    arr - the pointer pointing to the array to be sorted
 *    begin - the index of the start of the left half
 *    mid - the index separating left and right half
 *    end- the index of the end of the right half (exlusive)
 *    temparr - a pointer pointing to the temporary array that stores the results of merge
 */
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t* temparr) {
  int i = begin, j = mid;
  size_t len = end - begin;
  for (size_t k = 0; k < len; k++) {
    if (i >= mid) {
      temparr[k] = arr[j++];
    } else if (j >= end) {
      temparr[k] = arr[i++];
    } else if (arr[i] <= arr[j]) {
      temparr[k] = arr[i++];
    } else if (arr[i] >= arr[j]) {
      temparr[k] = arr[j++];
    }
  }
}

/**
 *  Set up for merging the sorted left half with the right half
 *  
 *  Parameters:
 *    arr - the pointer pointing to the array to be sorted
 *    begin - the index of the start of the left half
 *    mid - the index separating left and right half
 *    end- the index of the end of the right half (exlusive)
 */
void merge_together(int64_t *arr, size_t begin, size_t mid, size_t end) {
  size_t len = end - begin;
  int64_t* temp = (int64_t *) malloc(len * sizeof(int64_t));
  merge(arr, begin, mid, end, temp);
  for (int i = 0; i < len; ++i) {
    arr[begin + i] = temp[i];
  }
  free(temp);
}

/**
 *  Sort the array using the mergesort algorithm.
 *  
 *  Parameters:
 *    arr - the pointer pointing to the array to be sorted
 *    begin - the index of the start of the portion to be sorted
 *    end- the index of the end of the portion to be sorted (exlusive)
 *    threshold - the threshold below which the data will be sorted sequentially
 */
void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  size_t len = end - begin;
  if (len <= threshold) {
    qsort(arr + begin, len, sizeof(int64_t), compare_int64);
  } else if (begin < end) {
    size_t mid = (begin + end) / 2;
    pid_t pid_left = fork(); // First child process to sort the left half
    if (pid_left == -1) {
      fprintf(stderr, "Error: first fork failed to start a new process.\n");
      exit(1);
    } else if (pid_left == 0) {             
      merge_sort(arr, begin, mid, threshold);  
      exit(0);
    }
    pid_t pid_right = fork(); // Second child process to sort the right half
    if (pid_right == -1) {
      fprintf(stderr, "Error: second fork failed to start a new process.\n");
      exit(2);
    } else if (pid_right == 0) {              
      merge_sort(arr, mid, end, threshold);     
      exit(0);
    }
    int wstatus_left;
    pid_t actual_pid_left = waitpid(pid_left, &wstatus_left, 0);
    if (actual_pid_left == -1) {
      fprintf(stderr, "Error: waitpid_left fails.\n");
      exit(3);
    } else if (!WIFEXITED(wstatus_left)) {
      fprintf(stderr, "Error: subprocess for sorting the left half crashed, was interrupted, or did not exit normally.\n");
      exit(4);
    } else if (WEXITSTATUS(wstatus_left) != 0) {
      fprintf(stderr, "Error: subprocess for sorting the left half exited with error.\n");
      exit(5);
    }
    int wstatus_right;
    pid_t actual_pid_right = waitpid(pid_right, &wstatus_right, 0);
    if (actual_pid_right == -1) {
      fprintf(stderr, "Error: waitpid_right fails.\n");
      exit(6);
    } else if (!WIFEXITED(wstatus_right)) {
      fprintf(stderr, "Error: subprocess for sorting the right half crashed, was interrupted, or did not exit normally.\n");
      exit(7);
    } else if (WEXITSTATUS(wstatus_right) != 0) {
      fprintf(stderr, "Error: subprocess for sorting the right half exited with error.\n");
      exit(8);
    }
    merge_together(arr, begin, mid, end);
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
  if (end != argv[2] + strlen(argv[2])) {
    fprintf(stderr, "Error: the threshold value is invalid.\n");
    return 2;
  }
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Error: the file with the integers to be sorted can't be opened.\n");
    return 3;
  }
  // Use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: fstat error occurs.\n");
    return 4;
  }
  size_t file_size_in_bytes = statbuf.st_size;
  size_t len = file_size_in_bytes / sizeof(int64_t);
  // Map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    fprintf(stderr, "Error: mmap error occurs.\n");
    return 5;
  }
  // Sort the data!
  merge_sort(data, 0, len, threshold);
  // unmap and close the file
  int unmap = munmap(data, file_size_in_bytes);
  int closed = close(fd);
  if (closed != 0 || unmap != 0){
    fprintf(stderr, "Error: fail to unmap and close the file.\n");
    return 6;
  }
  // Exit with a 0 exit code if sort was successful
  return 0;
}
