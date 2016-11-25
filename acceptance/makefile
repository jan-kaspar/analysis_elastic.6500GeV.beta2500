GSL_DIR = /afs/cern.ch/cms/slc6_amd64_gcc481/external/gsl/1.10/

INCS = -I$(GSL_DIR)/include
LIBS = -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm

all: acceptance_test

acceptance_test: acceptance_test.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		$(INCS) $(LIBS) \
		acceptance_test.cc -o acceptance_test
