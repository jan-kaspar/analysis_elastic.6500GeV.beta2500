all: \
	make_ratios \
	extract_correction	

make_ratios: make_ratios.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		make_ratios.cc -o make_ratios

extract_correction: extract_correction.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		extract_correction.cc -o extract_correction
