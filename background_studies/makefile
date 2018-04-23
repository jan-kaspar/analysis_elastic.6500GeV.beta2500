all: \
	make_ratios

make_ratios: make_ratios.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		make_ratios.cc -o make_ratios
