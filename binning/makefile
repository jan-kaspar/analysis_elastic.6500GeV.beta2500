all: simu make_generators get_acceptance_correction make_fit

simu: simu.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -Wextra simu.cc -o simu

make_generators: make_generators.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -Wextra make_generators.cc -o make_generators

get_acceptance_correction: get_acceptance_correction.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -Wextra get_acceptance_correction.cc -o get_acceptance_correction

make_fit: make_fit.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -Wextra make_fit.cc -o make_fit
