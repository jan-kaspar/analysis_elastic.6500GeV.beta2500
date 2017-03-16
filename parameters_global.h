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
	anal.t_min = 8E-4; anal.t_max = 1.0;
	anal.t_min_full = 0.; anal.t_max_full = 1.1;

	// approximate (time independent) resolutions
	anal.si_th_y_1arm = 0.37E-6 / sqrt(2.);
	anal.si_th_y_1arm_unc = 0.02E-6 / sqrt(2.);

	anal.si_th_y_2arm = anal.si_th_y_1arm / sqrt(2.);
	anal.si_th_y_2arm_unc = 0E-6;

	anal.si_th_x_1arm_L = 12.5E-6 / sqrt(2.);
	anal.si_th_x_1arm_R = 12.5E-6 / sqrt(2.);
	anal.si_th_x_1arm_unc = 0E-6;

	anal.si_th_x_LRdiff = 12.5E-6;
	anal.si_th_x_LRdiff_unc = 0.3E-6;

	anal.si_th_y_LRdiff = anal.si_th_y_1arm * sqrt(2.);
	anal.si_th_y_LRdiff_unc = 0.007E-6;

	// TODO
	anal.si_th_x_2arm = 0.3E-6;
	anal.si_th_x_2arm_unc = 0E-6;

	// alignment-determination settings
	anal.alignment_t0 = 0.;			// beginning of the first time-slice
	anal.alignment_ts = 20.*60.;	// time-slice in s
	
	anal.alignmentYRanges["L_2_F"] = Analysis::AlignmentYRange(-20.0, -4.0, +4.0, +20.0);
	anal.alignmentYRanges["L_1_F"] = Analysis::AlignmentYRange(-16.0, -3.0, +3.0, +16.0);

	anal.alignmentYRanges["R_1_F"] = Analysis::AlignmentYRange(-16.0, -3.0, +3.0, +16.0);
	anal.alignmentYRanges["R_2_F"] = Analysis::AlignmentYRange(-20.0, -4.0, +4.0, +20.0);

	// correction settings
	anal.use_resolution_fits = true;

	anal.use_3outof4_efficiency_fits = true;
	anal.inefficiency_3outof4 = 0.;
	anal.inefficiency_shower_near = 0.03;	// probability of shower created in near and spreading to far: 2 * 1.5%

	anal.use_pileup_efficiency_fits = false;
	anal.inefficiency_pile_up = 0.;

	anal.inefficiency_trigger = 0.;
	anal.inefficiency_DAQ = 0.;

	anal.bckg_corr = 1.;

	// normalisation settings
	anal.L_int = 1.;	// mb^-1
}

//----------------------------------------------------------------------------------------------------

void Init_global_45b_56t()
{
	// fiducial cuts
	anal.fc_L_l = FiducialCut(4.1E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_R_l = FiducialCut(4.2E-6, -20E-6, -0.01, +20E-6, 0.05);
	anal.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_G_l = FiducialCut(4.3E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);

	// unfolding settings
	unsmearing_file = "unfolding_cf_45b_56t.root";
	unsmearing_object = "first_fit/g_t_corr";	// TODO: switch to histogram
}

//----------------------------------------------------------------------------------------------------

void Init_global_45t_56b()
{
	// fiducial cuts
	anal.fc_L_l = FiducialCut(4.1E-6, -12E-6, -0.05, +20E-6, +0.01);
	anal.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_R_l = FiducialCut(4.1E-6, -20E-6, -0.05, +15E-6, +0.04);
	anal.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_G_l = FiducialCut(4.2E-6, -12E-6, -0.05, +15E-6, +0.04);
	anal.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);

	// unfolding settings
	unsmearing_file = "unfolding_cf_45b_56t.root";
	unsmearing_object = "first_fit/g_t_corr";	// TODO: switch to histogram
}
