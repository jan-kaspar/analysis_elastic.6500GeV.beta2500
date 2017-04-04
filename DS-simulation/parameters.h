#include "parameters_global.h"

double timestamp_min = 0E3, timestamp_max = 3600. * 10;

void Init_base()
{
	// load global settings
	Init_global();

	simulated_dataset = true;
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = 0.E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = 0.E-6; anal.cut2_si = 0.4E-6;

	anal.L_int = 2.6701E+06;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = 0.E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = 0.E-6; anal.cut2_si = 0.4E-6;

	anal.L_int = 2.6701E+06;
}
