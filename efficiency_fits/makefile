all: global_fit

global_fit: global_fit.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		global_fit.cc -o global_fit
