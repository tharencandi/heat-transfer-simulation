
CC = gcc
CFLAGS = -std=gnu99 -O3 -fopenmp -Wall

all: heatmap sharing
debug_heatmap: heatmap.c utility.h algorithm.c
	$(CC) $(CFLAGS) -g $< -o $@

heatmap: heatmap.c utility.h algorithm.c
	$(CC) $(CFLAGS) $< -o $@

sharing: sharing.c utility.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f heatmap sharing debug_heatmap
