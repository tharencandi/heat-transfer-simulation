/*
============================================================================
Filename    : algorithm.c
Author      : Herman Thong, Tharen Candi
SCIPER		: 359559, 359399
============================================================================
*/
#include <math.h>
// elements with same j but different i are far apart.
//elements with same i but different j are close.
#define INPUT(I,J) input[(I)*length+(J)]
#define OUTPUT(I,J) output[(I)*length+(J)]
#define BLOCK_SIZE 8
void simulate(double *input, double *output, int threads, int length, int iterations)
{

    /*
        data is brought into cache BY THE BLOCK. therefore, its optimal to access neigbouring elements
        within a block to prevent cache misses. 


        finding: llc misses are coming from the n iterations::

        is it possible to perform n iterations on the cacheblock when the cache block is loaded. 

        can we also incorporate unrolling?

    */
    double *temp;
    omp_set_num_threads(threads);
    // Parallelize this!!
    #pragma omp parallel shared(output)
    for(int n=0; n < iterations; n++)
    {   
        #pragma omp for
        for(int jj = 0; jj <= length - BLOCK_SIZE+2; jj += BLOCK_SIZE-2) {
            for(int kk = 0; kk <= length- BLOCK_SIZE+2; kk += BLOCK_SIZE-2) {
               
                for (int i = jj+1; i < BLOCK_SIZE + jj -1 ; i ++ ) {
                    for (int j = kk+1; j < BLOCK_SIZE + kk -1; j++) {
                      
                        if ( ((i == length/2-1) || (i== length/2))
                        && ((j == length/2-1) || (j == length/2)) )
                        continue;

                        OUTPUT(i,j) = (INPUT(i-1,j-1) + INPUT(i-1,j) + INPUT(i-1,j+1) +
                                    INPUT(i,j-1)   + INPUT(i,j)   + INPUT(i,j+1)   +
                                    INPUT(i+1,j-1) + INPUT(i+1,j) + INPUT(i+1,j+1) )/9;
                    }
                }
                // printf("count %d\n", count * sizeof(int)); = 4 elems / 16bytes 
                
            }
        }
        

        temp = input;
        input = output;
        output = temp;
    
    }
}


void simulate_base(double *input, double *output, int threads, int length, int iterations) {
    double *temp;
    omp_set_num_threads(threads);
    // Parallelize this!!
    #pragma omp parallel shared(output)
    for(int n=0; n < iterations; n++)
    {   
        #pragma omp for
        for(int i=1; i<length-1; i++)
        {
            for(int j=1; j<length-1; j++)
            {
                    if ( ((i == length/2-1) || (i== length/2))
                        && ((j == length/2-1) || (j == length/2)) )
                        continue;

                    OUTPUT(i,j) = (INPUT(i-1,j-1) + INPUT(i-1,j) + INPUT(i-1,j+1) +
                                   INPUT(i,j-1)   + INPUT(i,j)   + INPUT(i,j+1)   +
                                   INPUT(i+1,j-1) + INPUT(i+1,j) + INPUT(i+1,j+1) )/9;
            }
        }

        temp = input;
        input = output;
        output = temp;
    }
}