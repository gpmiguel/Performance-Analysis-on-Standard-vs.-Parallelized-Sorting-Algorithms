#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/sysinfo.h>

// global variables

int N;
int *arr;

typedef struct ARG{
	int start;
	int end;
	int iteration;
	int *array;
}args;

// for checking if array is initialized correctly, and if array has been sorted correctly
void printArray(int *arr){
  int i;

  for (i = 0; i < N; ++i){
    printf("%d ", arr[i]);
  }

  printf("\n");
}

// swapping function
void swap(int *x, int *y){
  int temp = *x;
  *x = *y;
  *y = temp;
}

// actual sorting algorithm
void * parallelBubbleSort(void *argument){
  int i, j, tempNum;

  args * temp = (args *) argument;

  for (i = temp->start; i < (temp->end)-1; i++){
      for (j = temp->start; j < (temp->end)-1; j++){
        if (temp->array[j] > temp->array[j+1]){
        	swap(&(temp->array[j]), &(temp->array[j+1]));
        } 
      }
  }

  pthread_exit(&temp);     
}

// setting randomized values ranging from [-N, N] inclusive
void randomize(int *arr){
  int i;
  for (i = 0; i < N; ++i){
    arr[i] = (rand() % (N)) - (N/2); 
  }
}

int get_cpu_count(){
    return get_nprocs_conf();
}

// Driver program to test above functions
int main(){

  FILE *fp;

  fp = fopen("pBubbleSort.csv", "w+");
  fprintf(fp, "N,seconds\n");

  args * arguments;
  pthread_t *tid;
  int **temp;

  for (N = 1000; N != 150000; N = N + 1000)
  {
    arr = (int *) malloc(sizeof(int) * N);
    int partitions = get_cpu_count();
    int i, j;

    arguments = (args *) malloc(sizeof(args) * partitions);
    tid = (pthread_t *) malloc(partitions * sizeof(pthread_t));

    randomize(arr);

    temp = (int **) malloc(sizeof(int *) * partitions);
    for(i = 0; i < partitions; i++){
      temp[i] = (int *) malloc(sizeof(int) * (N));
    }

    int start = 0;
    int iterator = 0; 
    int end = (N/partitions);

    struct timespec t1, t2;
    long sec, nano;
    double elapsed;

    clock_gettime(CLOCK_REALTIME, &t1);

    while(iterator != partitions){
      arguments[iterator].start = start;
      arguments[iterator].end = end;
      arguments[iterator].iteration = start;
      arguments[iterator].array = (int *) malloc(sizeof(int) * N);

      for(j = arguments[iterator].start; j < arguments[iterator].end; j++){
        temp[iterator][j] = arr[j];
      }

      arguments[iterator].array = temp[iterator];

      cpu_set_t cpuset; 

      //the CPU we whant to use
      int cpu = iterator;

      CPU_ZERO(&cpuset);       //clears the cpuset
      CPU_SET(cpu, &cpuset);  // sets CPU

      pthread_create(&(tid[iterator]), NULL, parallelBubbleSort, (void *) &arguments[iterator]);
      pthread_setaffinity_np(tid[iterator], sizeof(cpuset), &cpuset);

      start += (N/partitions);
      end += (N/partitions);
      iterator++;
    }

    for (int i = 0; i < partitions; i++){
      pthread_join(tid[i], NULL); // to wait for thread execution to finish for each node before proceeding
    }

    // free(tid);

    // merging the parallely sorted arrays
    for (i = 0; i < N; i++){
      int smallest = (N+1);
      int index;
      for(j = 0; j < partitions; j++){
        if (arguments[j].array[arguments[j].iteration] < smallest && arguments[j].iteration != arguments[j].end){
          smallest = arguments[j].array[arguments[j].iteration];
          index = j;
        }
      }

      arr[i] = smallest;
      arguments[index].iteration += 1;
    }

    clock_gettime(CLOCK_REALTIME, &t2);

    sec = t2.tv_sec - t1.tv_sec;
    nano = t2.tv_nsec - t1.tv_nsec;
    elapsed = sec + nano*1e-9;

    printf("Time elapsed for Parallel Bubble Sort at N = %d is %f seconds\n\n", N, elapsed);
    fprintf(fp, "%d,%f\n", N, elapsed);

    free(arr);
    free(arguments);
    for(i = 0; i < partitions; i++){
      free(temp[i]);
    }
    free(temp);
  }

  for (N = 150000; N != 550000; N = N + 50000)
  {
    arr = (int *) malloc(sizeof(int) * N);
    int partitions = get_cpu_count();
    int i, j;

    arguments = (args *) malloc(sizeof(args) * partitions);
    tid = (pthread_t *) malloc(partitions * sizeof(pthread_t));

    randomize(arr);

    temp = (int **) malloc(sizeof(int *) * partitions);
    for(i = 0; i < partitions; i++){
      temp[i] = (int *) malloc(sizeof(int) * (N));
    }

    int start = 0;
    int iterator = 0; 
    int end = (N/partitions);

    struct timespec t1, t2;
    long sec, nano;
    double elapsed;

    clock_gettime(CLOCK_REALTIME, &t1);

    while(iterator != partitions){
      arguments[iterator].start = start;
      arguments[iterator].end = end;
      arguments[iterator].iteration = start;
      arguments[iterator].array = (int *) malloc(sizeof(int) * N);

      for(j = arguments[iterator].start; j < arguments[iterator].end; j++){
        temp[iterator][j] = arr[j];
      }

      arguments[iterator].array = temp[iterator];

      cpu_set_t cpuset; 

      //the CPU we whant to use
      int cpu = iterator;

      CPU_ZERO(&cpuset);       //clears the cpuset
      CPU_SET(cpu, &cpuset);  // sets CPU

      pthread_create(&(tid[iterator]), NULL, parallelBubbleSort, (void *) &arguments[iterator]);
      pthread_setaffinity_np(tid[iterator], sizeof(cpuset), &cpuset);

      start += (N/partitions);
      end += (N/partitions);
      iterator++;
    }

    for (int i = 0; i < partitions; i++){
      pthread_join(tid[i], NULL); // to wait for thread execution to finish for each node before proceeding
    }

    // free(tid);

    // merging the parallely sorted arrays
    for (i = 0; i < N; i++){
      int smallest = (N+1);
      int index;
      for(j = 0; j < partitions; j++){
        if (arguments[j].array[arguments[j].iteration] < smallest && arguments[j].iteration != arguments[j].end){
          smallest = arguments[j].array[arguments[j].iteration];
          index = j;
        }
      }

      arr[i] = smallest;
      arguments[index].iteration += 1;
    }

    clock_gettime(CLOCK_REALTIME, &t2);

    sec = t2.tv_sec - t1.tv_sec;
    nano = t2.tv_nsec - t1.tv_nsec;
    elapsed = sec + nano*1e-9;

    printf("Time elapsed for Parallel Bubble Sort at N = %d is %f seconds\n\n", N, elapsed);
    fprintf(fp, "%d,%f\n", N, elapsed);

    free(arr);
    free(arguments);
    for(i = 0; i < partitions; i++){
      free(temp[i]);
    }
    free(temp);
  }
 
  

  return 0;
}