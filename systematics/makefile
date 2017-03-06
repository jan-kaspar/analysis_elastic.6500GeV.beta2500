GSL_DIR="/afs/cern.ch/cms/slc6_amd64_gcc481/external/gsl/1.10/"

first: \
	numerical_integration numerical_integration_full matrix_numerical_integration

% : %.cc common.h ../NumericalIntegration.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		-I$(GSL_DIR)/include -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm \
		$< -o $@
