#include "parameters_global.h"

double timestamp_min = 1.5E3, timestamp_max = 32.0E3;

void Init_base()
{
	// load global settings
	Init_global();
	env.UseMatchedOptics();

	// selection of bunches
	keepAllBunches = true;
	//bunchMap[94882].push_back(0);

	// approximate (time independent) resolutions
	anal.si_th_y_1arm = 0.37E-6 / sqrt(2.);

	anal.si_th_x_LRdiff = 12.5E-6;
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = 0.4E-6; anal.cut1_si = 3E-6;
	anal.cut2_a = 1.; anal.cut2_c = -0.02E-6; anal.cut2_si = 0.5E-6;
	
	anal.cut7_a = 0.; anal.cut7_c = -0.007; anal.cut7_si = 0.3;

	// fiducial cuts
	anal.fc_L_l = FiducialCut(4.1E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_R_l = FiducialCut(4.2E-6, -20E-6, -0.01, +20E-6, 0.05);
	anal.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_G_l = FiducialCut(4.3E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = -0.2E-6; anal.cut1_si = 3.0E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.01E-6; anal.cut2_si = 0.5E-6;
	
	anal.cut7_a = 0.; anal.cut7_c = 0.02; anal.cut7_si = 0.3;

	// fiducial cuts
	anal.fc_L_l = FiducialCut(4.1E-6, -12E-6, -0.05, +20E-6, +0.01);
	anal.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_R_l = FiducialCut(4.1E-6, -20E-6, -0.05, +15E-6, +0.04);
	anal.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_G_l = FiducialCut(4.2E-6, -12E-6, -0.05, +15E-6, +0.04);
	anal.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);
}
