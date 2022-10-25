/*
============================================================================
Filename    : algorithm.c
Author      : Your names go here
SCIPER      : Your SCIPER numbers

============================================================================
*/
#include <math.h>

#define INPUT(I,J) input[(I)*length+(J)]
#define OUTPUT(I,J) output[(I)*length+(J)]
#define BLOCK_SIZE 4
void simulate(double *input, double *output, int threads, int length, int iterations)
{
    double *temp;
     omp_set_num_threads(threads);
    // Parallelize this!!
    for(int n=0; n < iterations; n++)
    {   
        #pragma omp parallel for shared(output)
        for(int jj = 0; jj < length -3; jj += BLOCK_SIZE/2) {
            for(int kk = 0; kk < length- 3; kk += BLOCK_SIZE/2) {
                
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
               



            }
        }
        


    /*
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
    */
        temp = input;
        input = output;
        output = temp;
    
    }
}
