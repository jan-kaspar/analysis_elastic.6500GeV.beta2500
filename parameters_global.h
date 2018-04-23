#include <string>
#include <vector>
#include <map>
#include <cmath>

double timestamp0 = 1474322400;

vector<string> distilledNtuples;

vector<AlignmentSource> alignmentSources;
Analysis anal;
Environment env;

string unsmearing_file;
string unsmearing_object;

string luminosity_data_file;

bool simulated_dataset = false;

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

	anal.si_th_y_LRdiff = anal.si_th_y_1arm * sqrt(2.);
	anal.si_th_y_LRdiff_unc = 0.007E-6;

	anal.si_th_y_2arm = anal.si_th_y_1arm / sqrt(2.);
	anal.si_th_y_2arm_unc = 0.01E-6;

	anal.si_th_x_1arm_L = 2E-6 / sqrt(2.);
	anal.si_th_x_1arm_R = 2E-6 / sqrt(2.);
	anal.si_th_x_1arm_unc = 0E-6;

	anal.si_th_x_LRdiff = 12.5E-6;
	anal.si_th_x_LRdiff_unc = 0.3E-6;

	anal.si_th_x_2arm = 0.26E-6;
	anal.si_th_x_2arm_unc = 0.04E-6;

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
	anal.fc_L_l = FiducialCut(4.4E-6, -50E-6, 0., -20E-6, 0.045);
	anal.fc_L_h = FiducialCut(102E-6, 0., 0., 0., 0.);

	anal.fc_R_l = FiducialCut(4.4E-6, -50E-6, 0., -12E-6, 0.045);
	anal.fc_R_h = FiducialCut(102E-6, 0., 0., 0., 0.);

	anal.fc_G_l = FiducialCut(4.6E-6, -50E-6, 0., -20E-6, 0.045);
	anal.fc_G_h = FiducialCut(100E-6, 0., 0., 0., 0.);

	// default tagging settings
	anal.cut1_a = 1.; anal.cut1_c = +0.097E-6; anal.cut1_si = 2.0E-6;
	anal.cut2_a = 1.; anal.cut2_c = -0.011E-6; anal.cut2_si = 0.39E-6;

	anal.cut5_a = 0.157370; anal.cut5_c = -0.006; anal.cut5_si = 0.0185;
	anal.cut6_a = 0.156567; anal.cut6_c = -0.006; anal.cut6_si = 0.023;

	anal.cut7_a = 495.; anal.cut7_c = -0.003; anal.cut7_si = 0.20;

	// unfolding settings
	unsmearing_file = "unfolding_cf_ni_45b_56t.root";
	unsmearing_object = "fit2-2/<binning>";
}

//----------------------------------------------------------------------------------------------------

void Init_global_45t_56b()
{
	// fiducial cuts
	anal.fc_L_l = FiducialCut(4.4E-6, +20E-6, -0.045, 50E-6, 0.);
	anal.fc_L_h = FiducialCut(102E-6, 0., 0., 0., 0.);

	anal.fc_R_l = FiducialCut(4.4E-6, +10E-6, -0.045, 50E-6, 0.);
	anal.fc_R_h = FiducialCut(102E-6, 0., 0.);

	anal.fc_G_l = FiducialCut(4.6E-6, 20E-6, -0.045, 50E-6, 0.);
	anal.fc_G_h = FiducialCut(100E-6, 0., 0., 0., 0.);

	// default tagging settings
	anal.cut1_a = 1.; anal.cut1_c = -0.235E-6; anal.cut1_si = 1.9E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.002E-6; anal.cut2_si = 0.39E-6;

	anal.cut5_a = 0.157848; anal.cut5_c = +0.001; anal.cut5_si = 0.019;
	anal.cut6_a = 0.156191; anal.cut6_c = -0.004; anal.cut6_si = 0.020;

	anal.cut7_a = 609.; anal.cut7_c = +0.004; anal.cut7_si = 0.19;

	// unfolding settings
	unsmearing_file = "unfolding_cf_ni_45t_56b.root";
	unsmearing_object = "fit2-2/<binning>";
}
