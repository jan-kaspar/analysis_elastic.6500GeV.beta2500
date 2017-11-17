#include "../../parameters.h"

#define USE_INIT_ADDITIONAL

void Init_additional()
{
	keepAllBunches = false;
	bunchList = { 10 };
	
	distilledNtuples = {
		"../../block1",
		"../../block2"
	};

	if (diagonal == d45b_56t)
	{
		anal.L_int = 3.3484E+04;
	}

	if (diagonal == d45t_56b)
	{
		anal.L_int = 3.3281E+04;
	}
}
