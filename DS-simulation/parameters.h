#include "parameters_global.h"

double timestamp_min = 0E3, timestamp_max = 1E3;

void Init_base()
{
	// load global settings
	Init_global();

	simulated_dataset = true;

	// same as in data analysis
	anal.use_resolution_fits = true;
	anal.use_3outof4_efficiency_fits = true;
	anal.use_pileup_efficiency_fits = false;
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = 0.E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = 0.E-6; anal.cut2_si = 0.4E-6;

	// TODO
	anal.L_int = 3.4516E+04;

	// TODO
	unsmearing_file = ""; // this disables applying unsmearing correction
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = 0.E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = 0.E-6; anal.cut2_si = 0.4E-6;

	// TODO
	anal.L_int = 3.4426E+04;

	// TODO
	unsmearing_file = ""; // this disables applying unsmearing correction
}
