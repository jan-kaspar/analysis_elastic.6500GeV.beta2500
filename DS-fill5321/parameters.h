#include "parameters_global.h"

double timestamp_min = 265.5E3, timestamp_max = 310.0E3;

void Init_base()
{
	// load global settings
	Init_global();

	env.UseMatchedOptics();

	// list of (sub-)directories with distilled ntuples
	distilledNtuples.push_back(".");

	// alignment settings
	/*
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atNone);
	alSrc.SetAlignmentB(atNone);
	alSrc.SetAlignmentC(atNone);

	alSrc.cnst.a_L_2_F = -0.3E-3; alSrc.cnst.b_L_2_F = -293.0E-3; alSrc.cnst.c_L_2_F = -144.3E-3; 
	alSrc.cnst.a_L_1_F = -0.7E-3; alSrc.cnst.b_L_1_F = -315.4E-3; alSrc.cnst.c_L_1_F = 109.9E-3; 
	alSrc.cnst.a_R_1_F = -2.2E-3; alSrc.cnst.b_R_1_F = -1035.3E-3; alSrc.cnst.c_R_1_F = 290.6E-3; 
	alSrc.cnst.a_R_2_F = -1.7E-3; alSrc.cnst.b_R_2_F = -104.4E-3; alSrc.cnst.c_R_2_F = -31.2E-3; 
	
	alignmentSources.push_back(alSrc);
	*/
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = +0.20E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.015E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 7.4727E+04;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.1E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.003E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 7.4408E+04;
}
