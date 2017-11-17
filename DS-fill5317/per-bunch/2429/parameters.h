#include "../../parameters.h"

#define USE_INIT_ADDITIONAL

void Init_additional()
{
	keepAllBunches = false;
	bunchList = { 2429 };
	
	distilledNtuples = {
		"../../block1",
		"../../block2"
	};

	if (diagonal == d45b_56t)
	{
		anal.L_int = 3.4022E+04;
	}

	if (diagonal == d45t_56b)
	{
		anal.L_int = 3.3931E+04;
	}
}
