all: print_fit

print_fit: print_fit.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` --std=c++11 print_fit.cc -o print_fit
