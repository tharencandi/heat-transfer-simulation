/*
 ============================================================================
 Filename    : utility.h
 Author      : Siddharth Gupta, Simla Harma
 ============================================================================
 */
#ifndef UTILITYH
#define UTILITYH

#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define INIT_VALUE 1000
#define x(i,j) x[(i)*length+(j)]

struct timeval start, end;

void set_clock()
{
	gettimeofday(&start, NULL);
}

double elapsed_time()
{
	gettimeofday(&end, NULL);
	double elapsed = (end.tv_sec - start.tv_sec); 
	elapsed += (double)(end.tv_usec - start.tv_usec) / 1000000.0;
	return elapsed;
}

//Initialize the two-dimensional array
void init(double *x, int length)
{
    memset(x, 0, sizeof(double)*length*length);
    x(length/2-1,length/2-1) = INIT_VALUE;
    x(length/2-1,length/2)   = INIT_VALUE;
    x(length/2,length/2-1)   = INIT_VALUE;
    x(length/2,length/2)     = INIT_VALUE;
}

//Save the two-dimensional array in a csv file
void save(double *x, int length, const char *outputfilename)
{
    FILE *output_file;
    output_file = fopen(outputfilename,"w");
    for(int i=0; i<length; i++)
    {
        for(int j=0; j<length-1; j++)
            fprintf(output_file, "%.4g;", x(i,j));
        fprintf(output_file, "%.4g", x(i,length-1));
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

typedef struct rand_gen {
	unsigned short * seed;
	double (*rand_func) (struct rand_gen);
	
} rand_gen;

// Gives a random number between 0 and 1
double next_rand(rand_gen gen){	
	return erand48(gen.seed);
}	

rand_gen init_rand(){
	unsigned short * seed_array = malloc (sizeof (unsigned short) * 3);
	seed_array[0] = 5;
	seed_array[1] = 10;
	seed_array[2] = omp_get_thread_num();

	rand_gen gen;
	gen.seed = seed_array;
	gen.rand_func = next_rand;

	return gen;
}

void free_rand(rand_gen gen){
	free(gen.seed);
}

#endif