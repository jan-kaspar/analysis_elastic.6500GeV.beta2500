GSL_DIR = /afs/cern.ch/cms/slc6_amd64_gcc481/external/gsl/1.10/

INCS = -I$(GSL_DIR)/include
LIBS = -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm

all: validation_vs_old mc

validation_vs_old: validation_vs_old.cc parameters.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		-I ../ $(INCS) $(LIBS) \
		validation_vs_old.cc -o validation_vs_old

mc: mc.cc parameters.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		-I ../ $(INCS) $(LIBS) \
		mc.cc -o mc
