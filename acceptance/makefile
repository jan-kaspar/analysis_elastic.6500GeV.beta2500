GSL_DIR = /afs/cern.ch/cms/slc6_amd64_gcc481/external/gsl/1.10/

INCS = -I$(GSL_DIR)/include
LIBS = -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm

all: acceptance_test mc

acceptance_test: acceptance_test.cc parameters.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		-I ../ $(INCS) $(LIBS) \
		acceptance_test.cc -o acceptance_test

mc: mc.cc parameters.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		-I ../ $(INCS) $(LIBS) \
		mc.cc -o mc
