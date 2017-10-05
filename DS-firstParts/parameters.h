#include "parameters_global.h"

double timestamp_min = 1.5E3, timestamp_max = 310.0E3;

void Init_base()
{
	// load global settings
	Init_global();

	env.UseMatchedOptics();

	// list of (sub-)directories with distilled ntuples
	distilledNtuples.push_back("../DS-fill5313");
	distilledNtuples.push_back("../DS-fill5314");
	distilledNtuples.push_back("../DS-fill5317/block1");
	distilledNtuples.push_back("../DS-fill5317/block2");
	distilledNtuples.push_back("../DS-fill5321");

	// list time periods
	anal.timeIntervals.push_back({1665, 2902});
	anal.timeIntervals.push_back({7645, 8862});
	anal.timeIntervals.push_back({10565, 12101});
	anal.timeIntervals.push_back({13998, 15426});
	anal.timeIntervals.push_back({17222, 18648});
	anal.timeIntervals.push_back({20493, 21993});
	anal.timeIntervals.push_back({23964, 25196});
	anal.timeIntervals.push_back({27043, 28642});
	anal.timeIntervals.push_back({30873, 31429});

	anal.timeIntervals.push_back({51423, 52567});
	anal.timeIntervals.push_back({55244, 55952});
	anal.timeIntervals.push_back({56692, 57105});
	anal.timeIntervals.push_back({58361, 59345});
	anal.timeIntervals.push_back({61353, 62347});
	anal.timeIntervals.push_back({64504, 65657});
	anal.timeIntervals.push_back({67268, 68814});
	anal.timeIntervals.push_back({71267, 72913});
	anal.timeIntervals.push_back({75037, 76894});
	anal.timeIntervals.push_back({79178, 80322});

	anal.timeIntervals.push_back({104904, 106240});
	anal.timeIntervals.push_back({108092, 109292});
	anal.timeIntervals.push_back({110973, 112259});
	anal.timeIntervals.push_back({113958, 115498});
	anal.timeIntervals.push_back({117429, 119047});
	anal.timeIntervals.push_back({121236, 122758});
	anal.timeIntervals.push_back({124698, 126321});
	anal.timeIntervals.push_back({128363, 130154});
	anal.timeIntervals.push_back({132283, 134535});
	anal.timeIntervals.push_back({137191, 139513});
	anal.timeIntervals.push_back({142280, 145258});
	anal.timeIntervals.push_back({148585, 151150});
	anal.timeIntervals.push_back({154377, 157256});
	anal.timeIntervals.push_back({160534, 162953});
	anal.timeIntervals.push_back({165742, 168343});
	anal.timeIntervals.push_back({171539, 174113});
	anal.timeIntervals.push_back({177147, 179834});
	anal.timeIntervals.push_back({182991, 185711});
	anal.timeIntervals.push_back({188972, 191741});
	anal.timeIntervals.push_back({194861, 197904});
	anal.timeIntervals.push_back({201335, 203842});

	anal.timeIntervals.push_back({265690, 267500});
	anal.timeIntervals.push_back({269677, 271161});
	anal.timeIntervals.push_back({273040, 275155});
	anal.timeIntervals.push_back({277720, 279652});
	anal.timeIntervals.push_back({282770, 285351});
	anal.timeIntervals.push_back({288101, 291401});
	anal.timeIntervals.push_back({295254, 298082});
	anal.timeIntervals.push_back({301563, 304136});
	anal.timeIntervals.push_back({307097, 308419});

	// alignment settings
	/*
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atNone);
	alSrc.SetAlignmentB(atNone);
	alSrc.SetAlignmentC(atNone);

	alSrc.cnst.a_L_2_F = -0.3E-3; alSrc.cnst.b_L_2_F = -271.3E-3; alSrc.cnst.c_L_2_F = -142.0E-3; 
	alSrc.cnst.a_L_1_F = -0.7E-3; alSrc.cnst.b_L_1_F = -290.6E-3; alSrc.cnst.c_L_1_F = 108.9E-3; 
	alSrc.cnst.a_R_1_F = -2.2E-3; alSrc.cnst.b_R_1_F = -1064.4E-3; alSrc.cnst.c_R_1_F = 251.8E-3; 
	alSrc.cnst.a_R_2_F = -1.8E-3; alSrc.cnst.b_R_2_F = -132.6E-3; alSrc.cnst.c_R_2_F = -85.1E-3; 
	
	alignmentSources.push_back(alSrc);
	*/
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = +0.17E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.009E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 1.4953E+05;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.17E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.002E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 1.4938E+05;
}
