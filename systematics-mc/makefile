GSL_DIR="/afs/cern.ch/cms/slc6_amd64_gcc481/external/gsl/1.10/"

first: \
	simu_direct

% : %.cc common.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		-I$(GSL_DIR)/include -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm \
		$< -o $@
