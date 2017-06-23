all: \
	make_fits \
	print_fits_all_rot \
	print_fits_best_rot

make_fits: make_fits.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		make_fits.cc -o make_fits

print_fits_best_rot: print_fits_best_rot.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		print_fits_best_rot.cc -o print_fits_best_rot

print_fits_all_rot: print_fits_all_rot.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		print_fits_all_rot.cc -o print_fits_all_rot
