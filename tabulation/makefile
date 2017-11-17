all: tabulate

tabulate : tabulate.cc
	g++ --std=c++11 `root-config --libs` `root-config --cflags` -O3 -g \
		tabulate.cc -o tabulate
