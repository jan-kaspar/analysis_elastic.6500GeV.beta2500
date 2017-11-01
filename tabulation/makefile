all: tabulate

tabulate : tabulate.cc
	g++ `root-config --libs` `root-config --cflags` -O3 -g \
		tabulate.cc -o tabulate
