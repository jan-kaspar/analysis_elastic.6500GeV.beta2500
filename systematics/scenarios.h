#include "TF1.h"

//----------------------------------------------------------------------------------------------------

struct BiasesPerArm
{
	// shifts (rad)
	//    th_y_reco = th_y_true + sh_th_y
	double sh_th_x = 0., sh_th_y = 0.;

	// tilts (rad)
	//    th_x_reco = th_x_true + tilt_th_x_eff_prop_to_th_y * th_y
	//    th_y_reco = th_y_true + tilt_th_y_eff_prop_to_th_x * th_x
	double tilt_th_x_eff_prop_to_th_y = 0.;
	double tilt_th_y_eff_prop_to_th_x = 0.;

	// scales
	//    th_y_reco = sc_th_y * th_y_true
	double sc_th_x = 1., sc_th_y = 1.;
};

//----------------------------------------------------------------------------------------------------

struct Biases
{
	// per-arm biases
	BiasesPerArm L, R;

	// errors of the inefficiency corrections (3/4 and 2/4)
	double eff_intercept = 0.;
	double eff_slope = 0.;	// rad^-1

	// normalisation error (relative factor)
	double norm = 0.;

	// non-gaussian distributions
	bool use_non_gaussian_d_x = false;
	bool use_non_gaussian_d_y = false;

	void Print() const
	{
		printf("left arm:\n");
		printf("    sh_th_x = %.3E, sh_th_y = %.3E\n", L.sh_th_x, L.sh_th_y);
		printf("    tilt_th_x_eff_prop_to_th_y = %.3E, tilt_th_y_eff_prop_to_th_x = %.3E\n", L.tilt_th_x_eff_prop_to_th_y, L.tilt_th_y_eff_prop_to_th_x);
		printf("    sc_th_x = %.3E, sc_th_y = %.3E\n", L.sc_th_x, L.sc_th_y);

		printf("right arm:\n");
		printf("    sh_th_x = %.3E, sh_th_y = %.3E\n", R.sh_th_x, R.sh_th_y);
		printf("    tilt_th_x_eff_prop_to_th_y = %.3E, tilt_th_y_eff_prop_to_th_x = %.3E\n", R.tilt_th_x_eff_prop_to_th_y, R.tilt_th_y_eff_prop_to_th_x);
		printf("    sc_th_x = %.3E, sc_th_y = %.3E\n", R.sc_th_x, R.sc_th_y);

		printf("global:\n");
		printf("    eff_intercept = %.3E, eff_slope = %.3E\n", eff_intercept, eff_slope);
		printf("    norm = %.3E\n", norm);
		printf("    use_non_gaussian_d_x = %u\n", use_non_gaussian_d_x);
		printf("    use_non_gaussian_d_y = %u\n", use_non_gaussian_d_y);
	}
};

//----------------------------------------------------------------------------------------------------

int SetScenario(const string &scenario, Biases &biases, Environment &env_sim, Analysis &anal_sim, Environment &env_rec, Analysis &anal_rec)
{
	if (scenario == "none")
	{
		return 0;
	}

	// ---------- x shift ----------

	{
		// v = sigma that corresponds to modes (L, R) = (+1, +1) or (+1, -1)
		//    this recovers single-arm sigma = 0.50 um
		const double v = 0.50E-6 / sqrt(2.);

		if (scenario == "sh-thx")
		{
			biases.L.sh_th_x = v;
			biases.R.sh_th_x = v;

			anal_rec.fc_L_l.th_x_m += v; anal_rec.fc_L_h.th_x_m += v;
			anal_rec.fc_L_l.th_x_p += v; anal_rec.fc_L_h.th_x_p += v;

			anal_rec.fc_R_l.th_x_m += v; anal_rec.fc_R_h.th_x_m += v;
			anal_rec.fc_R_l.th_x_p += v; anal_rec.fc_R_h.th_x_p += v;

			anal_rec.fc_G_l.th_x_m += v; anal_rec.fc_G_h.th_x_m += v;
			anal_rec.fc_G_l.th_x_p += v; anal_rec.fc_G_h.th_x_p += v;

			return 0;
		}

		if (scenario == "sh-thx-LRasym")
		{
			biases.L.sh_th_x = +v;
			biases.R.sh_th_x = -v;

			anal_rec.fc_L_l.th_x_m += +v; anal_rec.fc_L_h.th_x_m += +v;
			anal_rec.fc_L_l.th_x_p += +v; anal_rec.fc_L_h.th_x_p += +v;

			anal_rec.fc_R_l.th_x_m += -v; anal_rec.fc_R_h.th_x_m += -v;
			anal_rec.fc_R_l.th_x_p += -v; anal_rec.fc_R_h.th_x_p += -v;

			return 0;
		}
	}

	// ---------- y shift ----------

	if (scenario == "sh-thy")
	{
		const double v = 0.35E-6 / sqrt(2.);

		biases.L.sh_th_y = v;
		biases.R.sh_th_y = v;

		anal_rec.fc_L_l.th_y_0 += v * th_y_sign; anal_rec.fc_L_h.th_y_0 += v * th_y_sign;
		anal_rec.fc_R_l.th_y_0 += v * th_y_sign; anal_rec.fc_R_h.th_y_0 += v * th_y_sign;
		anal_rec.fc_G_l.th_y_0 += v * th_y_sign; anal_rec.fc_G_h.th_y_0 += v * th_y_sign;

		return 0;
	}

	if (scenario == "sh-thy-LRasym")
	{
		// typical value De^{R-L} th_y ~ 0.04 urad
		const double v = 0.04E-6 / 2.;

		biases.L.sh_th_y = +v;
		biases.R.sh_th_y = -v;

		anal_rec.fc_L_l.th_y_0 += +v * th_y_sign; anal_rec.fc_L_h.th_y_0 += +v * th_y_sign;
		anal_rec.fc_R_l.th_y_0 += -v * th_y_sign; anal_rec.fc_R_h.th_y_0 += -v * th_y_sign;

		return 0;
	}

	if (scenario == "sh-thy-TBuncor")
	{
		const double v = 0.017E-6 / sqrt(2.);

		biases.L.sh_th_y = v;
		biases.R.sh_th_y = v;

		anal_rec.fc_L_l.th_y_0 += v * th_y_sign; anal_rec.fc_L_h.th_y_0 += v * th_y_sign;
		anal_rec.fc_R_l.th_y_0 += v * th_y_sign; anal_rec.fc_R_h.th_y_0 += v * th_y_sign;
		anal_rec.fc_G_l.th_y_0 += v * th_y_sign; anal_rec.fc_G_h.th_y_0 += v * th_y_sign;

		return 0;
	}

	// TODO: in principle missing mode T-B uncorr, L-R antisym

	// ---------- xy tilt ----------

	{
		const double v_xy = 0.013 / sqrt(2.);
		const double v_yx = 0.00039 / sqrt(2.);

		// with the current values of v_xy, v_yx, the effect on the fiducial cuts in negligible
		// low bound: th_x ~ 20 urad, th_y ~ 4 urad
		//     th_x --> 20.04 urad, th_y --> 4.006 urad
		// high bound: th_x ~ 100 urad, th_y ~ 100 urad
		//     th_x --> 100.9 urad, th_y --> 100.03 urad

		if (scenario.compare("tilt-thx-thy") == 0)
		{
			biases.L.tilt_th_x_eff_prop_to_th_y = v_xy;
			biases.R.tilt_th_x_eff_prop_to_th_y = v_xy;

			biases.L.tilt_th_y_eff_prop_to_th_x = v_yx;
			biases.R.tilt_th_y_eff_prop_to_th_x = v_yx;

			return 0;
		}

		if (scenario.compare("tilt-thx-thy-LRasym") == 0)
		{
			biases.L.tilt_th_x_eff_prop_to_th_y = +v_xy;
			biases.R.tilt_th_x_eff_prop_to_th_y = -v_xy;

			biases.L.tilt_th_y_eff_prop_to_th_x = +v_yx;
			biases.R.tilt_th_y_eff_prop_to_th_x = -v_yx;

			return 0;
		}
	}

	// ---------- xy scaling (optics) ----------

	if (scenario.find("sc-thxy-mode") == 0)
	{
		const string mode = scenario.substr(12);

		double val_L_x=0., val_L_y, val_R_x, val_R_y;

		if (mode == "1") { val_L_x = -1.608E-03; val_L_y = +1.473E-03; val_R_x = -1.630E-03; val_R_y = +1.477E-03; }
		if (mode == "2") { val_L_x = -5.157E-04; val_L_y = +2.541E-05; val_R_x = +5.566E-04; val_R_y = +2.746E-05; }
		if (mode == "3") { val_L_x = +3.617E-04; val_L_y = +3.625E-04; val_R_x = +3.006E-04; val_R_y = +3.641E-04; }

		if (val_L_x != 0.)
		{
			biases.L.sc_th_x = 1. + val_L_x;
			biases.R.sc_th_x = 1. + val_R_x;
			biases.L.sc_th_y = 1. + val_L_y;
			biases.R.sc_th_y = 1. + val_R_y;

			anal_rec.fc_L_l.th_x_m *= (1. + val_L_x); anal_rec.fc_L_h.th_x_m *= (1. + val_L_x);
			anal_rec.fc_L_l.th_x_p *= (1. + val_L_x); anal_rec.fc_L_h.th_x_p *= (1. + val_L_x);

			anal_rec.fc_R_l.th_x_m *= (1. + val_R_x); anal_rec.fc_R_h.th_x_m *= (1. + val_R_x);
			anal_rec.fc_R_l.th_x_p *= (1. + val_R_x); anal_rec.fc_R_h.th_x_p *= (1. + val_R_x);

			const double val_G_x = (val_L_x + val_R_x) / 2.;
			anal_rec.fc_G_l.th_x_m *= (1. + val_G_x); anal_rec.fc_G_h.th_x_m *= (1. + val_G_x);
			anal_rec.fc_G_l.th_x_p *= (1. + val_G_x); anal_rec.fc_G_h.th_x_p *= (1. + val_G_x);

			return 0;
		}
	}

	// ---------- acceptance correction ----------

	if (scenario == "dx-sigma")
	{
		anal_sim.si_th_x_LRdiff += 0.3E-6;
		return 0;
	}

	if (scenario == "dy-sigma")
	{
		anal_sim.si_th_y_LRdiff += 0.007E-6;
		return 0;
	}

	if (scenario == "dx-non-gauss")
	{
		biases.use_non_gaussian_d_x = true;
		return 0;
	}

	// TODO: what about L-R asymmetry?

	// ---------- inefficiency correction ----------

	if (scenario == "eff-intercept")
	{
		// combination from 3/4 (0.003) and 2/4 (0.01)
		biases.eff_intercept = sqrt(0.003*0.003 + 0.01*0.01);
		return 0;
	}

	if (scenario == "eff-slope")
	{
		biases.eff_slope = 15.;
		return 0;
	}

	// ---------- beam momentum ----------

	if (scenario.compare("beam-mom") == 0)
	{
		env_rec.p *= (1. - 0.001);

		return 0;
	}

	// ---------- unsmearing ----------

	if (scenario == "mx-sigma")
	{
		anal_sim.si_th_x_2arm += 0.04E-6;
		return 0;
	}

	if (scenario == "my-sigma")
	{
		anal_sim.si_th_y_2arm += 0.010E-6;
		return 0;
	}

	// ---------- normalisation ----------

	if (scenario == "norm")
	{
		biases.norm = 0.05;
		return 0;
	}

	return 1;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void CalculateNonGaussianDistributionTransformation(TF1 *dist, double min, double max, double sigma0,
	double &et, double &al, double &be)
{
	const unsigned int n_points = 1000;
	const double step = (max - min) / (n_points - 1);

	double I0=0., I1=0., I2=0.;
	for (unsigned int i = 0; i < n_points; i++)
	{
		double x = min + i * step;
		double f = dist->Eval(x);

		I0 += f * step;
		I1 += f*x * step;
		I2 += f*x*x * step;
	}

	et = sqrt((I2 - I1*I1/I0) / sigma0/sigma0 / I0/I0/I0);
	al = et * I0;
	be = I1 / I0;
}

//----------------------------------------------------------------------------------------------------

TF1 *f_non_gaussian_dist_d_x = NULL;
double ngx_et, ngx_al, ngx_be;

void LoadNonGaussianDistributions(double si_d_x, double /* si_d_y */)
{
	TFile *f_in = TFile::Open("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/2rp/non-gaussianity/fit_dx.root");
	f_non_gaussian_dist_d_x = new TF1(* (TF1 *) f_in->Get("ff"));
	delete f_in;

	const double range = 6. * 15E-6;
	CalculateNonGaussianDistributionTransformation(f_non_gaussian_dist_d_x, -range, +range, si_d_x, ngx_et, ngx_al, ngx_be);
}

//----------------------------------------------------------------------------------------------------

double NonGauassianDistribution_d_x(double x)
{
	return f_non_gaussian_dist_d_x->Eval(ngx_al*(x + ngx_be)) * ngx_et;
}
