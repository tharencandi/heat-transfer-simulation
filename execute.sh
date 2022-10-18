#!/bin/bash
#SBATCH --chdir /scratch/<username>
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 16
#SBATCH --mem 2G


echo STARTING AT `date`

./sharing 1 100000000 32

./assignment2 1 10000 100 output.csv

echo FINISHED at `date`
