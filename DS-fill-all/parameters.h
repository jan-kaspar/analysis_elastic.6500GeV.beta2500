#include "parameters_global.h"

double timestamp_min = 104.5E3, timestamp_max = 206.5E3;

void Init_base()
{
	// load global settings
	Init_global();

	env.UseMatchedOptics();

	// list of (sub-)directories with distilled ntuples
	distilledNtuples.push_back("fill5313");
	distilledNtuples.push_back("fill5314");
	distilledNtuples.push_back("fill5317-block1");
	distilledNtuples.push_back("fill5317-block2");
	distilledNtuples.push_back("fill5321");
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// normalisation settings
	anal.L_int = 3.2469E+04 + 3.7761E+04 + 1.5004E+05 + 7.4103E+04;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// normalisation settings
	anal.L_int = 3.2449E+04 + 3.7734E+04 + 1.4966E+05 + 7.3826E+04;
}
