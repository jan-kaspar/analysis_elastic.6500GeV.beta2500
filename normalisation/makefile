all: \
	normalisation \
	uncertainty

normalisation: normalisation.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		normalisation.cc -o normalisation

uncertainty: uncertainty.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` -O3 -Wall -g \
		uncertainty.cc -o uncertainty
