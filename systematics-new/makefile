GSL_DIR="/afs/cern.ch/cms/slc6_amd64_gcc481/external/gsl/1.10/"

all: \
	mc_simu \
	mc_process \
	ni_simu \
	ni_process

% : %.cc
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		-I.. \
		-I$(GSL_DIR)/include -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm \
		$< -o $@

mc_simu : scenarios.h input_distributions.h command_line_tools.h \
		../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h \
		../DS-fill5313/parameters.h ../common.h ../parameters_global.h

ni_simu : scenarios.h input_distributions.h command_line_tools.h \
		../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h \
		../DS-fill5313/parameters.h ../common.h ../parameters_global.h
