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
#define OUTPUT(I,J) new_output[(I)*length_padded+(J)]

// #define INPUT(I,J) input[(I)*length+(J)]
// #define OUTPUT(I,J) output[(I)*length+(J)]

#define BLOCK_SIZE 8
#define BLOCK_SIZE_BYTES 64

double * padd_arr(double * in_arr, int length) {
    int r = length % BLOCK_SIZE;
    if (r != 0)
        r = BLOCK_SIZE - r;
    int num_elems = length * (length +r);


    //double * out_arr = (double*) malloc(sizeof(double) * num_elems);
    double * out_arr = NULL;
    if (posix_memalign((void**)&out_arr,BLOCK_SIZE_BYTES,sizeof(double) * num_elems) != 0) {
        printf("error allocting memory. quitting.");
        exit(1);
    }

    // add r between every index mulitple of length-1 and length
    for (int i = 0; i < length; i ++) {
        for (int j = 0; j < length; j++){
            out_arr[i*(length+r) + j] = in_arr[i*length + j];
        }
    }
   return out_arr;
}

void unpad_arr(double * in_arr, int out_length, double * out_arr) {
    int r = out_length % BLOCK_SIZE;
    if (r != 0)
        r = BLOCK_SIZE - r;

    int in_length = out_length + r;
    for(int i = 0; i < out_length; i ++) {
        for(int j = 0; j < out_length; j++) {
            out_arr[i*out_length + j] = in_arr[i*in_length + j];
            //printf("%d %d\n", i,j);
            
        }
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


    int r = length % BLOCK_SIZE;
    if (r != 0)
        r = BLOCK_SIZE - r;
    int length_padded = length + r;

    printf("length_padded: %d\n", length_padded);
    // length_padded = length;
    // new_input = input;
    // new_output = output;
    
    double *temp;
    int n;
    omp_set_num_threads(threads);
    //create threads once. Join threads once.
    #pragma omp parallel private(n)
    {
        for(n=0; n < iterations; n++)
        {   
            /*
                splitting by rows reduces false sharing.if two threads share same row but different columns
                then they could be writing to same cache block.

                With our array so that length is a multiple of cache block. All rows will be different cache block.
            */
            #pragma omp for schedule(static) 
            for(int ii= 0; ii<= length - BLOCK_SIZE + 2; ii += BLOCK_SIZE-2) {
                for(int jj = 0; jj <= length - BLOCK_SIZE + 2; jj += BLOCK_SIZE-2) {
                
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
                }
            }
            #pragma omp single
            {
                // temp = input;
                // input = output;
                // output = temp;

                temp = new_input;
                new_input = new_output;
                new_output = temp;

            }
        }

    }
    unpad_arr(new_input, length, input);
    unpad_arr(new_output, length, output);

    //free(new_output);
    //free(new_input);
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