#include <string>
#include <vector>
#include <map>
#include <cmath>

double timestamp0 = 1474322400;

vector<AlignmentSource> alignmentSources;
Analysis anal;
Environment env;

string unsmearing_file;
string unsmearing_object;

string luminosity_data_file;

//----------------------------------------------------------------------------------------------------

void Init_global()
{
	// environment settings
	env.InitNominal();

	// selection of bunches
	keepAllBunches = true;
	//bunchMap[94882].push_back(0);

	// binning
	anal.t_min = 20E-4; anal.t_max = 1.0;
	anal.t_min_full = 0.; anal.t_max_full = 1.1;

	// approximate (time independent) resolutions
	anal.si_th_y_1arm = 0.37E-6 / sqrt(2.);
	anal.si_th_y_1arm_unc = 0.02E-6 / sqrt(2.);

	anal.si_th_y_2arm = anal.si_th_y_1arm / sqrt(2.);
	anal.si_th_y_2arm_unc = 0E-6;

	anal.si_th_x_1arm_L = 2E-6 / sqrt(2.);
	anal.si_th_x_1arm_R = 2E-6 / sqrt(2.);
	anal.si_th_x_1arm_unc = 0E-6;

	anal.si_th_x_2arm = 0E-6;
	anal.si_th_x_2arm_unc = 0E-6;

	// fiducial cuts
	anal.th_x_lcut = -1.;	
	anal.th_x_hcut = +1.;

	// alignment-determination settings
	anal.alignment_t0 = 0.;			// beginning of the first time-slice
	anal.alignment_ts = 20.*60.;	// time-slice in s
	
	anal.alignmentYRanges["L_2_F"] = Analysis::AlignmentYRange(-20.0, -4.0, +4.0, +20.0);
	anal.alignmentYRanges["L_1_F"] = Analysis::AlignmentYRange(-16.0, -3.0, +3.0, +16.0);

	anal.alignmentYRanges["R_1_F"] = Analysis::AlignmentYRange(-16.0, -3.0, +3.0, +16.0);
	anal.alignmentYRanges["R_2_F"] = Analysis::AlignmentYRange(-20.0, -4.0, +4.0, +20.0);

	// correction settings
	anal.use_3outof4_efficiency_fits = true;
	anal.inefficiency_3outof4 = 0.;
	anal.inefficiency_shower_near = 0.03;	// 2 * 1.5%
	anal.use_pileup_efficiency_fits = false;
	anal.inefficiency_pile_up = 0.;

	anal.inefficiency_trigger = 0.;
	anal.inefficiency_DAQ = 0.;

	anal.bckg_corr = 1.;

	// unfolding settings
	// TODO
#if 0
	unsmearing_file = "";	// diagonal dependent
	//unsmearing_object = "cf,<binning>/exp3/corr_final";
	//unsmearing_object = "cf,<binning>/exp3+exp4/corr_final";
	unsmearing_object = "ff";
#endif
}

//----------------------------------------------------------------------------------------------------

void Init_global_45b_56t()
{
	// TODO
	anal.cut5_a = 0.107200; anal.cut5_c = -0.; anal.cut5_si = 0.016;
	anal.cut6_a = 0.105559; anal.cut6_c = -0.; anal.cut6_si = 0.019;

	// fiducial cuts
	anal.th_y_lcut_L = 4E-6; anal.th_y_lcut_R = 4E-6; anal.th_y_lcut = 5E-6;
	anal.th_y_hcut_L = 102E-6; anal.th_y_hcut_R = 102E-6; anal.th_y_hcut = 100E-6;

	anal.fc_L_l = FiducialCut(-20E-6, 4E-6, 0., 0.05);
	anal.fc_L_h = FiducialCut(0., 102E-6, 0., 0.);

	anal.fc_R_l = FiducialCut(-12E-6, 4.1E-6, 0., 0.05);
	anal.fc_R_h = FiducialCut(0., 102E-6, 0., 0.);

	anal.fc_G_l = FiducialCut(-20E-6, 4.2E-6, 0., 0.05);
	anal.fc_G_h = FiducialCut(0., 100E-6, 0., 0.);
}

//----------------------------------------------------------------------------------------------------

void Init_global_45t_56b()
{
	// TODO
	anal.cut5_a = 0.10671; anal.cut5_c = 0.; anal.cut5_si = 0.018;
	anal.cut6_a = 0.10564; anal.cut6_c = 0.; anal.cut6_si = 0.018;

	// fiducial cuts
	anal.th_y_lcut_L = 4E-6; anal.th_y_lcut_R = 4E-6; anal.th_y_lcut = 4.5E-6;
	anal.th_y_hcut_L = 102E-6; anal.th_y_hcut_R = 102E-6; anal.th_y_hcut = 100E-6;

	//anal.fc_L_l = FiducialCut(0., 4E-6, 0., 0.);
	anal.fc_L_l = FiducialCut(+15E-6, 4E-6, -0.05, 0.);
	anal.fc_L_h = FiducialCut(0., 102E-6, 0., 0.);

	//anal.fc_R_l = FiducialCut(0., 4E-6, 0., 0.);
	anal.fc_R_l = FiducialCut(+10E-6, 4E-6, -0.05, 0.);
	anal.fc_R_h = FiducialCut(0., 102E-6, 0., 0.);

	//anal.fc_G_l = FiducialCut(0., 4.5E-6, 0., 0.);
	anal.fc_G_l = FiducialCut(17E-6, 4.1E-6, -0.05, 0.);
	anal.fc_G_h = FiducialCut(0., 100E-6, 0., 0.);
}
