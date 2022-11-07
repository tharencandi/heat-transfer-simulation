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

#define INPUT(I,J) input[(I)*length+(J)]
#define OUTPUT(I,J) output[(I)*length+(J)]

void simulate(double *input, double *output, int threads, int length, int iterations)
{   
    double *temp;
    omp_set_num_threads(threads);
    //create threads once. Join threads once.
    #pragma omp parallel 
    {
    for(int n=0; n < iterations; n++)
    {   
        /*
                splitting by rows reduces false sharing.if two threads share same row but different columns
                then they could be writing to same cache block.

                With our array so that length is a multiple of cache block. All rows will be different cache block.
            */
        #pragma omp for schedule(static)
        for(int i=1; i< length-1; i++)
        {   
            // unroll loop by 2 to prevent overhead of this loop
            for(int j=1; j<length-(length%2); j += 2)
            {
                if (! (((i == length/2-1) || (i == length/2))
                    && ((j+1 == length/2-1) || (j+1 == length/2)))
                    && j < length - 1 )
                    OUTPUT(i,j+1) = (INPUT(i-1,j) + INPUT(i-1,j+1) + INPUT(i-1,j+2) +
                                INPUT(i,j)   + INPUT(i,j+1)   + INPUT(i,j+2)   +
                                INPUT(i+1,j) + INPUT(i+1,j+1) + INPUT(i+1,j+2) )/9;
                
                if (! (((i == length/2-1) || (i == length/2))
                    && ((j+2 == length/2-1) || (j+2 == length/2)))
                    && j+1 < length - 1 )
                    OUTPUT(i,j+2) = (INPUT(i-1,j+1) + INPUT(i-1,j+2) + INPUT(i-1,j+3) +
                                INPUT(i,j+1)   + INPUT(i,j+2)   + INPUT(i,j+3)   +
                                INPUT(i+1,j+1) + INPUT(i+1,j+2) + INPUT(i+1,j+3) )/9;
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

void simulate_base(double *input, double *output, int threads, int length, int iterations) {
    double *temp;
    omp_set_num_threads(threads);
    // #pragma omp parallel 
    {
    for(int n=0; n < iterations; n++)
    {   
        #pragma omp parallel for
        for(int i=1; i<length-1; i++)
        {     
            for(int j=1; j<length-1; j ++)
            {
                    if ( ((i == length/2-1) || (i== length/2))
                        && ((j == length/2-1) || (j == length/2)) )
                        continue;

                    OUTPUT(i,j) = (INPUT(i-1,j-1) + INPUT(i-1,j) + INPUT(i-1,j+1) +
                                   INPUT(i,j-1)   + INPUT(i,j)   + INPUT(i,j+1)   +
                                   INPUT(i+1,j-1) + INPUT(i+1,j) + INPUT(i+1,j+1) )/9;
            }
        }

        // #pragma omp single
        {
            temp = input;
            input = output;
            output = temp;

        }
    }
    }
}

void simulate_unroll(double *input, double *output, int threads, int length, int iterations) {
    double *temp;
    omp_set_num_threads(threads);
    // #pragma omp parallel 
    {
    for(int n=0; n < iterations; n++)
    {   
        #pragma omp parallel for schedule(guided, 64)
        for(int i=1; i< length-1; i++)
        {   
            // unroll loop by 2 to prevent overhead of this loop
            for(int j=1; j<length-(length%2); j += 2)
            {
                if (! (((i == length/2-1) || (i == length/2))
                    && ((j+1 == length/2-1) || (j+1 == length/2)))
                    && j < length - 1 )
                    OUTPUT(i,j+1) = (INPUT(i-1,j) + INPUT(i-1,j+1) + INPUT(i-1,j+2) +
                                INPUT(i,j)   + INPUT(i,j+1)   + INPUT(i,j+2)   +
                                INPUT(i+1,j) + INPUT(i+1,j+1) + INPUT(i+1,j+2) )/9;
                
                if (! (((i == length/2-1) || (i == length/2))
                    && ((j+2 == length/2-1) || (j+2 == length/2)))
                    && j+1 < length - 1 )
                    OUTPUT(i,j+2) = (INPUT(i-1,j+1) + INPUT(i-1,j+2) + INPUT(i-1,j+3) +
                                INPUT(i,j+1)   + INPUT(i,j+2)   + INPUT(i,j+3)   +
                                INPUT(i+1,j+1) + INPUT(i+1,j+2) + INPUT(i+1,j+3) )/9;
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

