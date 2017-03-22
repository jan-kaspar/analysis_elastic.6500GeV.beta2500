all: make_fits

make_fits: make_fits.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -Wextra \
		make_fits.cc -o make_fits
