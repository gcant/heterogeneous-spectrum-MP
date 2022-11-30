CPP=g++
CFLAGS=-O3 -fopenmp -march=native -std=c++17 -I include/eigen3  -I include -fno-math-errno
SONAME=-soname

ifeq ($(shell uname -s),Darwin)
	SONAME=-install_name
	CPP=g++-12
endif

main:
	$(CPP) heterogeneous.cpp -o heterogeneous $(CFLAGS)

py_wrap:
	$(CPP) $(CFLAGS)  -c -fPIC py_wrapper/python-heterogeneous.cpp -o py_wrapper/het.o  $(CFLAGS)
	$(CPP) $(CFLAGS) -shared -Wl,$(SONAME),het.so -o py_wrapper/het.so py_wrapper/het.o $(CFLAGS)
