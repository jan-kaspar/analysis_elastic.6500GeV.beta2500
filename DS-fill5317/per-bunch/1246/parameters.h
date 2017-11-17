#include "../../parameters.h"

#define USE_INIT_ADDITIONAL

void Init_additional()
{
	keepAllBunches = false;
	bunchList = { 1246 };
	
	distilledNtuples = {
		"../../block1",
		"../../block2"
	};

	if (diagonal == d45b_56t)
	{
		anal.L_int = 2.9120E+04;
	}

	if (diagonal == d45t_56b)
	{
		anal.L_int = 2.9038E+04;
	}
}
