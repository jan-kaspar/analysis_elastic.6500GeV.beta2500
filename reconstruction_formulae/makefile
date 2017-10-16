first: test_formulae_graph test_formulae_correlation print_optics_uncertainties

test_formulae_graph: test_formulae_graph.cc formulae.h ../common_definitions.h ../common_parameters.h ../common_algorithms.h ../stat.h common.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -g -O3 -Wall -Wextra \
		test_formulae_graph.cc -o test_formulae_graph

test_formulae_correlation: test_formulae_correlation.cc formulae.h ../common_definitions.h ../common_parameters.h ../common_algorithms.h ../stat.h common.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -g -O3 -Wall -Wextra \
		test_formulae_correlation.cc -o test_formulae_correlation

print_optics_uncertainties: print_optics_uncertainties.cc formulae.h ../common_definitions.h ../common_parameters.h ../common_algorithms.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -g -O3 -Wall -Wextra \
		print_optics_uncertainties.cc -o print_optics_uncertainties
