#include "parameters_global.h"

/**
 * IMPORTANT: keep synchronised with parameters.h in the analysis.
 **/

void Init_base()
{
	// load global settings
	Init_global();

	// list of (sub-)directories with distilled ntuples
	distilledNtuples.push_back(".");
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = 0.2E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = 0.000E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 3.4429E+04;

	// fiducial cuts
	/*
	anal.fc_L_l = FiducialCut(4.2E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_R_l = FiducialCut(4.2E-6, -20E-6, -0.01, +20E-6, 0.05);
	anal.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_G_l = FiducialCut(4.3E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);
	*/
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.1E-6; anal.cut1_si = 14E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.000E-6; anal.cut2_si = 0.38E-6;

	anal.L_int = 3.4381E+04;

	// fiducial cuts
	/*
	anal.fc_L_l = FiducialCut(4.2E-6, -12E-6, -0.05, +20E-6, +0.01);
	anal.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_R_l = FiducialCut(4.2E-6, -20E-6, -0.05, +15E-6, +0.04);
	anal.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_G_l = FiducialCut(4.3E-6, -12E-6, -0.05, +15E-6, +0.04);
	anal.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);
	*/
}
