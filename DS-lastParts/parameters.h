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
	anal.timeIntervals.push_back({2903, 4138});
	anal.timeIntervals.push_back({8863, 10079});
	anal.timeIntervals.push_back({12102, 13637});
	anal.timeIntervals.push_back({15427, 16853});
	anal.timeIntervals.push_back({18649, 20073});
	anal.timeIntervals.push_back({21994, 23493});
	anal.timeIntervals.push_back({25197, 26428});
	anal.timeIntervals.push_back({28643, 30240});
	anal.timeIntervals.push_back({31430, 31984});

	anal.timeIntervals.push_back({52568, 53711});
	anal.timeIntervals.push_back({55953, 56660});
	anal.timeIntervals.push_back({57106, 57517});
	anal.timeIntervals.push_back({59346, 60328});
	anal.timeIntervals.push_back({62348, 63341});
	anal.timeIntervals.push_back({65658, 66810});
	anal.timeIntervals.push_back({68815, 70360});
	anal.timeIntervals.push_back({72914, 74558});
	anal.timeIntervals.push_back({76895, 78750});
	anal.timeIntervals.push_back({80323, 81466});

	anal.timeIntervals.push_back({106241, 107576});
	anal.timeIntervals.push_back({109293, 110492});
	anal.timeIntervals.push_back({112260, 113545});
	anal.timeIntervals.push_back({115499, 117038});
	anal.timeIntervals.push_back({119048, 120664});
	anal.timeIntervals.push_back({122759, 124279});
	anal.timeIntervals.push_back({126322, 127943});
	anal.timeIntervals.push_back({130155, 131944});
	anal.timeIntervals.push_back({134536, 136787});
	anal.timeIntervals.push_back({139514, 141834});
	anal.timeIntervals.push_back({145259, 148235});
	anal.timeIntervals.push_back({151151, 153714});
	anal.timeIntervals.push_back({157257, 160135});
	anal.timeIntervals.push_back({162954, 165371});
	anal.timeIntervals.push_back({168344, 170943});
	anal.timeIntervals.push_back({174114, 176687});
	anal.timeIntervals.push_back({179835, 182520});
	anal.timeIntervals.push_back({185712, 188431});
	anal.timeIntervals.push_back({191742, 194510});
	anal.timeIntervals.push_back({197905, 200947});
	anal.timeIntervals.push_back({203843, 206348});

	anal.timeIntervals.push_back({267501, 269310});
	anal.timeIntervals.push_back({271162, 272644});
	anal.timeIntervals.push_back({275156, 277270});
	anal.timeIntervals.push_back({279653, 281584});
	anal.timeIntervals.push_back({285352, 287932});
	anal.timeIntervals.push_back({291402, 294701});
	anal.timeIntervals.push_back({298083, 300909});
	anal.timeIntervals.push_back({304137, 306709});
	anal.timeIntervals.push_back({308420, 309741});

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

	anal.L_int = 1.4595E+05;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.17E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.002E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 1.4517E+05;
}
