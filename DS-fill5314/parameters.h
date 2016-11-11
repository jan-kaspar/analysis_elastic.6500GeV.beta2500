#include "parameters_global.h"

double timestamp_min = 51.0E3, timestamp_max = 81.5E3;

void Init_base()
{
	// load global settings
	Init_global();

	// selection of bunches
	keepAllBunches = true;
	//bunchMap[94882].push_back(0);

	// alignment settings
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atConstant);
	alSrc.SetAlignmentB(atConstant);
	alSrc.SetAlignmentC(atConstant);

	alSrc.cnst.a_L_2_F = -0.2E-3; alSrc.cnst.b_L_2_F = -180.1E-3; alSrc.cnst.c_L_2_F = -149.4E-3; 
	alSrc.cnst.a_L_1_F = -0.4E-3; alSrc.cnst.b_L_1_F = -186.0E-3; alSrc.cnst.c_L_1_F = 105.7E-3; 
	alSrc.cnst.a_R_1_F = -2.5E-3; alSrc.cnst.b_R_1_F = -1206.0E-3; alSrc.cnst.c_R_1_F = 263.9E-3; 
	alSrc.cnst.a_R_2_F = -1.7E-3; alSrc.cnst.b_R_2_F = -243.5E-3; alSrc.cnst.c_R_2_F = -62.9E-3; 
	
	alignmentSources.push_back(alSrc);

#if 0
	anal.use_time_dependent_resolutions = false;

	anal.use_3outof4_efficiency_fits = false;
	anal.use_pileup_efficiency_fits = false;
	anal.inefficiency_3outof4 = 0.;				// diagonal dependent
	anal.inefficiency_shower_near = 0.03;
	anal.inefficiency_pile_up = 0.;				// diagonal dependent
	anal.inefficiency_trigger = 0.;

	anal.bckg_corr = 1.;
	
	anal.L_int_eff = 0.;	// mb^-1, diagonal dependent
	
	anal.eff_th_y_min = 200E-6; // TODO
	
	anal.t_min_fit = 0.027; // TODO
#endif

#if 0
	// TODO
	anal.alignment_t0 = 0.;			// beginning of the first time-slice
	anal.alignment_ts = 5.*60.;		// time-slice in s
	
	// TODO
	anal.alignmentYRanges["L_2_F"] = Analysis::AlignmentYRange(-22.0, -10.0, 9.0, 22.0);
	anal.alignmentYRanges["L_1_F"] = Analysis::AlignmentYRange(-19.3, - 8.8, 8.0, 19.6);

	anal.alignmentYRanges["R_1_F"] = Analysis::AlignmentYRange(-20.0, - 7.4, 8.4, 20.0);
	anal.alignmentYRanges["R_2_F"] = Analysis::AlignmentYRange(-23.0, - 8.2, 9.6, 24.0);
#endif

#if 0
	// TODO
	unsmearing_file = "";	// diagonal dependent
	//unsmearing_object = "cf,<binning>/exp3/corr_final";
	//unsmearing_object = "cf,<binning>/exp3+exp4/corr_final";
	unsmearing_object = "ff";

	// TODO
	luminosity_data_file = "../fill_3549_lumiCalc2.py_V04-02-04_lumibylsXing.csv";
#endif
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = +0.1E-6; anal.cut1_si = 3E-6;
	anal.cut2_a = 1.; anal.cut2_c = +0.02E-6; anal.cut2_si = 0.5E-6;
	
	// TODO
	/*
	anal.cut5_a = 0.107200; anal.cut5_c = -0.010; anal.cut5_si = 0.016;
	anal.cut6_a = 0.105559; anal.cut6_c = -0.002; anal.cut6_si = 0.019;
	*/

	anal.cut7_a = 0.; anal.cut7_c = +0.002; anal.cut7_si = 0.3;

#if 0
	// TODO
	//unsmearing_file = "unfolding_fit_45b_56t_old.root";

	anal.inefficiency_3outof4 = 0.0; // TODO
	anal.inefficiency_pile_up = 0.0; // TODO

	anal.L_int_eff = 79.42E3;	// TODO	
#endif
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	// TODO
	anal.cut1_a = 1.; anal.cut1_c = -0.2E-6; anal.cut1_si = 3.0E-6;
	anal.cut2_a = 1.; anal.cut2_c = -0.02E-6; anal.cut2_si = 0.5E-6;

	/*
	anal.cut5_a = 0.10671; anal.cut5_c = 0.; anal.cut5_si = 0.018;
	anal.cut6_a = 0.10564; anal.cut6_c = 0.; anal.cut6_si = 0.018;
	*/

	anal.cut7_a = 0.; anal.cut7_c = -0.007; anal.cut7_si = 0.3;

#if 0
	// TODO
	//unsmearing_file = "unfolding_fit_45b_56t_old.root";

	anal.inefficiency_3outof4 = 0.0; // TODO
	anal.inefficiency_pile_up = 0.0; // TODO

	anal.L_int_eff = 79.42E3;	// TODO	
#endif
}
