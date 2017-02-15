#include "parameters_global.h"

double timestamp_min = 104.5E3, timestamp_max = 206.5E3;

void Init_base()
{
	// load global settings
	Init_global();

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
	anal.cut2_a = 1.; anal.cut2_c = -0.01E-6; anal.cut2_si = 0.5E-6;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.17E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.01E-6; anal.cut2_si = 0.5E-6;
}
