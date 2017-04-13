all: fit_cuts

fit_cuts: fit_cuts.cc
	g++ --std=c++11 `root-config --libs` `root-config --cflags` -g -O3 -Wall -Wextra \
		fit_cuts.cc -o fit_cuts
