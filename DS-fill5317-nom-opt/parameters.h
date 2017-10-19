#include "parameters_global.h"

double timestamp_min = 104.5E3, timestamp_max = 206.5E3;

void Init_base()
{
	// load global settings
	Init_global();

	// list of (sub-)directories with distilled ntuples
	distilledNtuples.push_back("block1");
	distilledNtuples.push_back("block2");

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
	anal.cut1_a = 1.; anal.cut1_c = +0.10E-6; anal.cut1_si = 2.0E-6;
	anal.cut2_a = 1.; anal.cut2_c = -0.018E-6; anal.cut2_si = 0.39E-6;
	
	anal.cut5_a = 0.157370; anal.cut5_c = -0.006; anal.cut5_si = 0.0185;
	anal.cut6_a = 0.156567; anal.cut6_c = -0.006; anal.cut6_si = 0.023;

	anal.cut7_a = -1379; anal.cut7_c = -0.003; anal.cut7_si = 0.20;

	// normalisation settings
	anal.L_int = 1.4745E+05;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.23E-6; anal.cut1_si = 1.9E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.009E-6; anal.cut2_si = 0.39E-6;
	
	anal.cut5_a = 0.157848; anal.cut5_c = -0.000; anal.cut5_si = 0.019;
	anal.cut6_a = 0.156191; anal.cut6_c = -0.004; anal.cut6_si = 0.020;

	anal.cut7_a = -1275; anal.cut7_c = +0.005; anal.cut7_si = 0.19;

	// normalisation settings
	anal.L_int = 1.4709E+05;
}
