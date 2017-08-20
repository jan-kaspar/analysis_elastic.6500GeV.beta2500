#include "parameters_global.h"

double timestamp_min = 51.0E3, timestamp_max = 81.5E3;

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

	alSrc.cnst.a_L_2_F = -0.2E-3; alSrc.cnst.b_L_2_F = -180.1E-3; alSrc.cnst.c_L_2_F = -149.4E-3; 
	alSrc.cnst.a_L_1_F = -0.4E-3; alSrc.cnst.b_L_1_F = -186.0E-3; alSrc.cnst.c_L_1_F = 105.7E-3; 
	alSrc.cnst.a_R_1_F = -2.5E-3; alSrc.cnst.b_R_1_F = -1206.0E-3; alSrc.cnst.c_R_1_F = 263.9E-3; 
	alSrc.cnst.a_R_2_F = -1.7E-3; alSrc.cnst.b_R_2_F = -243.5E-3; alSrc.cnst.c_R_2_F = -62.9E-3; 
	
	alignmentSources.push_back(alSrc);
	*/
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = +0.2E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.015E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 3.9830E+04;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.1E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = -0.010E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 3.9833E+04;
}
