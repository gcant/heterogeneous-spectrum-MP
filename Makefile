CC=g++
CFLAGS=-O3 -fopenmp -march=native -std=c++17 -I include/eigen3  -I include

main:
	$(CC) heterogeneous.cpp -o heterogeneous $(CFLAGS)

