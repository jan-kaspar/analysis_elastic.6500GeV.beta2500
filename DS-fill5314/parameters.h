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
	anal.cut1_a = 1.; anal.cut1_c = +0.047E-6; anal.cut1_si = 2.0E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.016E-6; anal.cut2_si = 0.39E-6;
	
	anal.cut5_a = 0.157364; anal.cut5_c = +0.009; anal.cut5_si = 0.0185;
	anal.cut6_a = 0.156546; anal.cut6_c = -0.009; anal.cut6_si = 0.023;

	anal.cut7_a = 497.; anal.cut7_c = +0.007; anal.cut7_si = 0.20;

	// normalisation settings
	anal.L_int = 3.6354E+04;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.298E-6; anal.cut1_si = 1.9E-6;
	anal.cut2_a = 1.; anal.cut2_c = -0.029E-6; anal.cut2_si = 0.39E-6;

	anal.cut5_a = 0.157856; anal.cut5_c = -0.006; anal.cut5_si = 0.019;
	anal.cut6_a = 0.156048; anal.cut6_c = -0.009; anal.cut6_si = 0.020;

	anal.cut7_a = 555.; anal.cut7_c = -0.005; anal.cut7_si = 0.19;

	// normalisation settings
	anal.L_int = 3.6329E+04;
}
