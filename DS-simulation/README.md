 1. In the base directory run `./run simulate_distill.cc DS-simulation/`. For a first try, you may want to decrease the event count in `simulate_distill.cc`, for example to 4E5.
 1. In `DS-simulation/parameters.h` uncomment lines:
```
	anal.use_resolution_fits = false;
	unsmearing_file = "";
```
 1. Run `./run distributions.cc DS-simulation/`.
 1. Run `./run resolution_fit.cc DS-simulation/`.
 1. Run `./run unfolding_cf_ni.cc DS-simulation/`.
 1. Comment out the lines uncommented in the 2nd step.
 1. Run `./run distributions.cc DS-simulation/`.
 1. `cd normalisation; make && ./normalisation`. Then update `anal.L_int` values in `DS-simulation/parameters.h`.
 1. Run `./run distributions.cc DS-simulation/`.
 1. Finally produce and check plots in `plots/chain_validation`.
