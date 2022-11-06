/*
============================================================================
Filename    : algorithm.c
Author      : Herman Thong, Tharen Candi
SCIPER		: 359559, 359399
============================================================================
*/
#include <math.h>
#include <stdlib.h>
// elements with same j but different i are far apart.
//elements with same i but different j are close.
#define INPUT(I,J) new_input[(I)*length_padded+(J)]
#define ADDR(I,J,A) A[(I)*length+(J)];
#define OUTPUT(I,J) new_output[(I)*length_padded+(J)]
#define BLOCK_SIZE 8

double * padd_arr(double * in_arr, int length) {
    int r = BLOCK_SIZE - (length % BLOCK_SIZE);
    int num_elems = length * (length +r);
    double * out_arr = (double*) malloc(sizeof(double) * num_elems);
    
    // add r between every index mulitple of length-1 and length
 

    for (int i = 0; i < length; i ++) {
        for (int j = 0; j < length+r; j++){
            out_arr[i*(length+r) + j] = in_arr[i*length + j];
        }
    }
   return out_arr;
}

void unpad_arr(double * in_arr, int length, double * out_arr) {
    int r = BLOCK_SIZE - (length % BLOCK_SIZE);
    int num_elems = length * length;

    int row_count = 0;
    for(int i = 0; i < num_elems; i ++) {
        if (i % length == 0) //skip to next row? 
        {
            row_count ++;
        }

        out_arr[i] = in_arr[i + (r * row_count)];
    }

}


void simulate(double *input, double *output, int threads, int length, int iterations)
{
    

    /*
        data is brought into cache BY THE BLOCK. therefore, its optimal to access neigbouring elements
        within a block to prevent cache misses. 


        finding: llc misses are coming from the n iterations::

        is it possible to perform n iterations on the cacheblock when the cache block is loaded. 

        can we also incorporate unrolling?

    */
    double * new_input;
    double * new_output;
    new_input = padd_arr(input, length);
    new_output = padd_arr(output, length);
    int length_padded = length + (length % BLOCK_SIZE);

    double *temp;
    int n;
    omp_set_num_threads(threads);
    //#pragma omp parallel shared(output) //create threads once. Join threads once.
    #pragma omp parallel private(n)
    {
    for(n=0; n < iterations; n++)
    {   
        #pragma omp for schedule(static)
        for(int ii= 0; ii<= length - BLOCK_SIZE + 2; ii += BLOCK_SIZE-2) {
            //#pragma omp for 
            for(int jj = 0; jj <= length_padded - BLOCK_SIZE + 2; jj += BLOCK_SIZE-2) {
               
                for (int i = ii+1; i < BLOCK_SIZE + ii-1 ; i ++ ) {
                    for (int j = jj+1; j < BLOCK_SIZE + jj -1; j++) {
                      
                        if ( ((i == length/2-1) || (i == length/2))
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
        #pragma omp single
        {
            temp = input;
            input = output;
            output = temp;

        }
    }
    }
    unpad_arr(new_output, length, output);
}


/*
void simulate_base(double *input, double *output, int threads, int length, int iterations) {
    double *temp;
    omp_set_num_threads(threads);
    int n;
    // Parallelize this!!
    #pragma omp parallel private(n)
    {
    for(n=0; n < iterations; n++)
    {   
        #pragma omp for schedule(static)
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

        #pragma omp single
        {
            temp = input;
            input = output;
            output = temp;

        }
    }
    }
}
*/