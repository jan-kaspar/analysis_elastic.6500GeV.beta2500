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
	}
};

//----------------------------------------------------------------------------------------------------

int SetScenario(const string &scenario, Biases &biases, Environment &env_sim, Analysis &anal_sim, Environment &env_rec, Analysis &anal_rec)
{
	if (scenario == "none")
	{
		return 0;
	}

	if (scenario == "sh-thx")
	{
		const double v = 0.5E-6;
		biases.L.sh_th_x = v;
		biases.R.sh_th_x = v;
		return 0;
	}

	if (scenario == "sh-thx-LRasym")
	{
		const double v = 0.5E-6;
		biases.L.sh_th_x = +v;
		biases.R.sh_th_x = -v;
		return 0;
	}

	if (scenario == "sh-thy")
	{
		const double v = 0.29E-6;
		biases.L.sh_th_y = v;
		biases.R.sh_th_y = v;
		return 0;
	}

	if (scenario == "sh-thy-LRasym")
	{
		// "typical" De^{R-L} th_y ~ 0.03 urad
		const double v = 0.03E-6 / 2.;
		biases.L.sh_th_y = +v;
		biases.R.sh_th_y = -v;
		return 0;
	}

	if (scenario.compare("tilt-thx-thy") == 0)
	{
		const double v_xy = 0.0055;
		biases.L.tilt_th_x_eff_prop_to_th_y = v_xy;
		biases.R.tilt_th_x_eff_prop_to_th_y = v_xy;

		const double v_yx = 0.00046;
		biases.L.tilt_th_y_eff_prop_to_th_x = v_yx;
		biases.R.tilt_th_y_eff_prop_to_th_x = v_yx;

		return 0;
	}

	if (scenario.compare("tilt-thx-thy-LRasym") == 0)
	{
		const double v_xy = 0.0055;
		biases.L.tilt_th_x_eff_prop_to_th_y = +v_xy;
		biases.R.tilt_th_x_eff_prop_to_th_y = -v_xy;

		const double v_yx = 0.00046;
		biases.L.tilt_th_y_eff_prop_to_th_x = +v_yx;
		biases.R.tilt_th_y_eff_prop_to_th_x = -v_yx;

		return 0;
	}

	if (scenario.compare("sc-thx") == 0)
	{
		// TODO: preliminary
		const double v = 1E-03;
		biases.L.sc_th_x = 1. - v;
		biases.R.sc_th_x = 1. - v;

		return 0;
	}

	if (scenario.compare("sc-thx-LRasym") == 0)
	{
		// TODO: preliminary
		const double v = 1E-03;
		biases.L.sc_th_x = 1. - v;
		biases.R.sc_th_x = 1. + v;

		return 0;
	}

	if (scenario.compare("sc-thy") == 0)
	{
		// TODO: preliminary
		const double v = 1E-03;
		biases.L.sc_th_y = 1. - v;
		biases.R.sc_th_y = 1. - v;

		return 0;
	}

	if (scenario.compare("sc-thy-LRasym") == 0)
	{
		// TODO: preliminary
		const double v = 1E-03;
		biases.L.sc_th_y = 1. - v;
		biases.R.sc_th_y = 1. + v;

		return 0;
	}

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

	if (scenario == "eff-intercept")
	{
		// combination from 3/4 (0.003) and 2/4 (0.01)
		biases.eff_intercept = sqrt(0.01*0.01 + 0.003*0.003);
		return 0;
	}

	if (scenario == "eff-slope")
	{
		biases.eff_slope = 15.;
		return 0;
	}

	if (scenario.compare("beam-mom") == 0)
	{
		env_rec.p *= (1. - 0.001);

		return 0;
	}

	if (scenario == "norm")
	{
		biases.norm = 0.1;
		return 0;
	}

	return 1;
}
