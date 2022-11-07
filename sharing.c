/*
============================================================================
Filename    : sharing.c
Author      : Herman Thong, Tharen Candi
SCIPER		: 359559, 359399
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#define BLK_SIZE 64
#define RANDGENSIZE 16

int perform_buckets_computation(int, int, int);

int main (int argc, const char *argv[]) {
    int num_threads, num_samples, num_buckets;
   
    if (argc != 4) {
		printf("Invalid input! Usage: ./sharing <num_threads> <num_samples> <num_buckets> \n");
		return 1;
	} else {
        num_threads = atoi(argv[1]);
        num_samples = atoi(argv[2]);
        num_buckets = atoi(argv[3]);
	}

    set_clock();
    
    perform_buckets_computation(num_threads, num_samples, num_buckets);

    printf("Using %d threads: %d operations completed in %.4gs.\n", num_threads, num_samples, elapsed_time());
    return 0;
}

typedef struct {
    rand_gen generator;
    char _padding[BLK_SIZE - RANDGENSIZE];
} paddedRand;

int perform_buckets_computation(int num_threads, int num_samples, int num_buckets) {
    
    omp_set_num_threads(num_threads);
    // final histogram
    volatile int *histogram = (int*) calloc(num_buckets, sizeof(int));

    //create list of histograms ptrs - one for each thread.
    int **tmp_hist = (int**)calloc(num_threads, sizeof(int*));

    // create a histogram for each thread and store ptr in tmp_hist array.
    // this prevent true sharing when writing to a bucket.
    // to prevent false sharing, these arrays MUST BE some multiple of cacheline (BLK_SIZE)
    // the arrays must also be ALLIGNED to size of cacheline (BLK_SIZE). This means that
    // the head of the array wil be loaded into L1 at the start of a cache block.
    
    for (int i = 0; i < num_threads; i ++) {
        size_t size = (num_buckets * sizeof(int));
        size += BLK_SIZE - (size%BLK_SIZE); // round UP to multiple of blk size.
        
        //allocate memory alligned to BLK_SIZE
        if (posix_memalign((void**)&tmp_hist[i],BLK_SIZE, size) != 0) {
            printf("error allocating memory. early stopping.\n");
            exit(1);
        }        
    }

    // each thread adds to their priv hist
    #pragma omp parallel shared (tmp_hist) 
    {   //no need for private generators. they are read only data structures.
        rand_gen r = init_rand();

        int tid = omp_get_thread_num();

        #pragma omp for
        for(int i = 0; i < num_samples; i++) {
            int bucket = next_rand(r) * num_buckets;
            tmp_hist[tid][bucket] ++;
        }

       free_rand(r);
        
    }

    //merge - paritions buckets to threads. 
    #pragma omp parallel for shared (tmp_hist, histogram)
    for(int i = 0; i < num_buckets; i ++) {
        for (int tid = 0; tid < num_threads; tid++) {
            histogram[i] += tmp_hist[tid][i];
        }
    }


    // free memory
    for (int i = 0; i < num_threads; i++) {
        free((void*)tmp_hist[i]);
    }
    free((void*) tmp_hist);
    return 0;
}

// int perform_buckets_computation_naive(int num_threads, int num_samples, int num_buckets) {
//     volatile int *histogram = (int*) calloc(num_buckets, sizeof(int));
//     rand_gen generator = init_rand();
//     omp_set_num_threads(num_threads);
//     #pragma omp parallel for
//     for(int i = 0; i < num_samples; i++){
//         int val = next_rand(generator) * num_buckets;
//         histogram[val]++;
//     }
//     free_rand(generator);
//     return 0;
// }