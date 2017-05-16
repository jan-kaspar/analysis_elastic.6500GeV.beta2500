struct BiasesPerArm
{
	// reconstruction shifts (rad)
	double sh_th_x = 0., sh_th_y = 0.;

	// reconstruction tilt (rad)
	double thx_thy_tilt = 0.;

	// reconstruction scales, th_y_reco = sc_th_y * th_y_true
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

	/*
	// relative error of beam momentum
	double de_rel_p = 0.;	

	// errors of unsmearing parameters
	double si_d_x_err = 0.;
	double si_d_y_err = 0.;
	*/

	void Print() const
	{
		printf("left arm:\n");
		printf("    sh_th_x = %.3E, sh_th_y = %.3E, thx_thy_tilt = %.3E\n", L.sh_th_x, L.sh_th_y, L.thx_thy_tilt);
		printf("    sc_th_x = %.3E, sc_th_y = %.3E\n", L.sc_th_x, L.sc_th_y);

		printf("right arm:\n");
		printf("    sh_th_x = %.3E, sh_th_y = %.3E, thx_thy_tilt = %.3E\n", R.sh_th_x, R.sh_th_y, R.thx_thy_tilt);
		printf("    sc_th_x = %.3E, sc_th_y = %.3E\n", R.sc_th_x, R.sc_th_y);

		printf("global:\n");
		printf("    eff_intercept = %.3E, eff_slope = %.3E\n", eff_intercept, eff_slope);
		printf("    norm = %.3E\n", norm);
		// TODO
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
		biases.L.sh_th_x = biases.R.sh_th_x = 0.5E-6;
		return 0;
	}

	if (scenario == "sh-thy")
	{
		biases.L.sh_th_y = biases.R.sh_th_y = 0.24E-6;
		return 0;
	}

	if (scenario.compare("tilt-thx-thy") == 0)
	{
		biases.L.thx_thy_tilt = biases.R.thx_thy_tilt = 0.005;
		return 0;
	}

	if (scenario.compare("opt-m1") == 0)
	{
		// TODO: preliminary
		biases.L.sc_th_x = biases.R.sc_th_x = 1. - 1E-03;
		biases.L.sc_th_y = biases.R.sc_th_y = 1.;

		return 0;
	}

	if (scenario.compare("opt-m2") == 0)
	{
		// TODO: preliminary
		biases.L.sc_th_x = biases.R.sc_th_x = 1.;
		biases.L.sc_th_y = biases.R.sc_th_y = 1. - 1E-03;

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
