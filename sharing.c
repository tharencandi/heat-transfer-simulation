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
    
    volatile int *histogram = (int*) calloc(num_buckets, sizeof(int));

    int **tmp_hist = (int**)calloc(num_threads, sizeof(int*));
    for (int i = 0; i < num_threads; i ++) {
        tmp_hist[i] = (int*) calloc(num_buckets, sizeof(int));
    }
    //int tmp_hist[num_threads][num_buckets];
    
    // each thread adds to their priv hist
    #pragma omp parallel shared (tmp_hist) 
    {   
        paddedRand g;
        init_rand(&g.generator);
        int tid = omp_get_thread_num();
        printf("%p gen %p with seed: %d\n", &g, &g.generator, g.generator.seed[2]);
        #pragma omp for
        for(int i = 0; i < num_samples; i++) {
           
            int val = next_rand(g.generator) * num_buckets;
            
            tmp_hist[tid][val] ++;
           
        }
       free_rand(g.generator);
       
        
    }

    //merge
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