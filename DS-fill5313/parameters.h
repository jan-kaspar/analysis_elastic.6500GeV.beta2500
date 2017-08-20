#include "parameters_global.h"

double timestamp_min = 1.5E3, timestamp_max = 32.0E3;

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

	alSrc.cnst.a_L_2_F = -0.2E-3; alSrc.cnst.b_L_2_F = -273.3E-3; alSrc.cnst.c_L_2_F = -196.6E-3; 
	alSrc.cnst.a_L_1_F = -0.3E-3; alSrc.cnst.b_L_1_F = -296.1E-3; alSrc.cnst.c_L_1_F = 57.4E-3; 
	alSrc.cnst.a_R_1_F = -1.7E-3; alSrc.cnst.b_R_1_F = -940.0E-3; alSrc.cnst.c_R_1_F = 272.2E-3; 
	alSrc.cnst.a_R_2_F = -1.6E-3; alSrc.cnst.b_R_2_F = -24.2E-3; alSrc.cnst.c_R_2_F = -62.4E-3; 
	
	alignmentSources.push_back(alSrc);
	*/
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = 0.331E-6; anal.cut1_si = 2.0E-6;
	anal.cut2_a = 1.; anal.cut2_c = -0.020E-6; anal.cut2_si = 0.39E-6;
	
	anal.cut5_a = 0.157361; anal.cut5_c = -0.014; anal.cut5_si = 0.0185;
	anal.cut6_a = 0.156780; anal.cut6_c = -0.003; anal.cut6_si = 0.023;

	anal.cut7_a = 623; anal.cut7_c = -0.004; anal.cut7_si = 0.20;

	// normalisation settings
	anal.L_int = 3.1259E+04;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.292E-6; anal.cut1_si = 1.9E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.000E-6; anal.cut2_si = 0.39E-6;
	
	anal.cut5_a = 0.157846; anal.cut5_c = -0.001; anal.cut5_si = 0.019;
	anal.cut6_a = 0.155975; anal.cut6_c = +0.000; anal.cut6_si = 0.020;

	anal.cut7_a = 561; anal.cut7_c = 0.022; anal.cut7_si = 0.19;

	// normalisation settings
	anal.L_int = 3.1240E+04;
}
