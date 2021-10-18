CC=g++
CFLAGS=-O3 -fopenmp -march=native -std=c++17 -I include/eigen3  -I include -fno-math-errno

main:
	$(CC) heterogeneous.cpp -o heterogeneous $(CFLAGS)

py_wrap:
	$(CC) $(CFLAGS)  -c -fPIC py_wrapper/python-heterogeneous.cpp -o py_wrapper/het.o  $(CFLAGS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,het.so -o py_wrapper/het.so py_wrapper/het.o $(CFLAGS)
