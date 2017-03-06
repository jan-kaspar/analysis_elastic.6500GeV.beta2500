struct Mode
{
	string label;
	pen p;

	string mc_dir_file;
	string mc_dir_tag;
	real mc_dir_ref;

	string num_int_file, num_int_tag;
	real num_int_ref;

	string mat_num_int_tag;
};

//----------------------------------------------------------------------------------------------------

Mode[] modes;

//----------------------------------------------------------------------------------------------------

void AddMode(string _l, pen _p = black,
	string _mdf, string _mdt, real _mdr,
	string _nif, string _nit, real _nir,
	string _mnit)
{
	Mode m;
	
	m.label = _l;
	m.p = _p;

	m.mc_dir_file = _mdf;
	m.mc_dir_tag = _mdt;
	m.mc_dir_ref = _mdr;
	
	m.num_int_file = _nif;
	m.num_int_tag = _nit;
	m.num_int_ref = _nir;

	m.mat_num_int_tag = _mnit;

	modes.push(m);
}

//----------------------------------------------------------------------------------------------------

void AddAllModes()
{
	AddMode("\vbox{\hbox{alignment:}\hbox{shift in $\th_x^*$}}", black,
		"sd", "alig-sh-thx", 1,
		"s", "alig-sh-thx", 1,
		"alig-sh-thx"
	);

	AddMode("\vbox{\hbox{alignment:}\hbox{shift in $\th_y^*$}}", red,
		"sd", "alig-sh-thy:D+0,R+1", 1,
		"s", "alig-sh-thy", 1,
		"alig-sh-thy:D+0,R+1"
	);

	//--------------------

	AddMode("\vbox{\hbox{alignment + optics:}\hbox{$x$-$y$ tilt}}", darkred,
		"sd", "alig-tilt", 1,
		"s", "thx-thy-tilt", 1,
		"thx-thy-tilt"
	);

	//--------------------

	AddMode("\vbox{\hbox{optics:}\hbox{$\th_{x, y}^*$ scaling -- mode 1}}", heavygreen,
		"sd", "opt-m1", 1,
		"s", "opt-m1", 1,
		"opt-scale-m1"
	);

	AddMode("\vbox{\hbox{optics:}\hbox{$\th_{x, y}^*$ scaling -- mode 2}}", cyan,
		"sd", "opt-m2", 1,
		"s", "opt-m2", 1,
		"opt-scale-m2"
	);
	
	//--------------------
	
	AddMode("\vbox{\hbox{acceptance correction:}\hbox{unc.~of beam divergence~RMS}}", blue+dashed,
		"sd", "acc-corr-sigma-unc", 1,
		"d", "x+0,y+0/<TDIST>/sigma offset/systematic effect", 1,
		"acc-corr-sigma-unc"
	);

	AddMode("\vbox{\hbox{acceptance correction:}\hbox{beam divergence L-R asymmetry}}", blue+dashed,
		"sd", "acc-corr-sigma-asym", 1,
		"d", "x+0,y+0/<TDIST>/sigma asymmetry/systematic effect", 1,
		"acc-corr-sigma-asym"
	);
	
	AddMode("\vbox{\hbox{acceptance correction:}\hbox{beam divergence non-gaussianity}}", blue+dashed,
		"sd", "acc-corr-non-gauss", 1,
		"d", "x+0,y+0/<TDIST>/non-gauss/systematic effect", 1,
		"acc-corr-non-gauss"
	);

	//--------------------
	
	AddMode("\vbox{\hbox{3-out-of-4 efficiency:}\hbox{slope uncertainty}}", black+dashed,
		"sd", "eff-slp", 1,
		"s", "eff-slp", 1,
		"eff-slp"
	);

	//--------------------
	
	AddMode("\vbox{\hbox{beam momentum:}\hbox{offset}}", magenta,
		"sd", "beam-mom", 1,
		"s", "beam-mom", 1,
		"beam-mom"
	);

	//--------------------

	AddMode("\vbox{\hbox{unfolding:}\hbox{x smearing dependence}}", blue+dashed,
		"sdu", "unsmearing correction/<TDIST>/unsm_corr_unc_th_x", 0,
		"d", "unsmearing correction/<TDIST>/unsm_corr_unc_th_x", 0,
		"unsm-sigma-x"
	);

	AddMode("\vbox{\hbox{unfolding:}\hbox{y smearing dependence}}", blue+dashed,
		"sdu", "unsmearing correction/<TDIST>/unsm_corr_unc_th_y", 0,
		"d", "unsmearing correction/<TDIST>/unsm_corr_unc_th_y", 0,
		"unsm-sigma-y"
	);

	AddMode("\vbox{\hbox{unfolding:}\hbox{model dependence}}", blue+dashed,
		"sdu", "unsmearing correction/unsm_corr_unc_model", 0,
		"d", "unsmearing correction/unsm_corr_unc_model", 0,
		"unsm-model"
	);

	//--------------------

	AddMode("\vbox{\hbox{normalisation:}\hbox{luminosity and efficiencies}}", magenta+dashed,
		"sd", "norm", 1,
		"s", "norm", 1,
		"norm"
	);
}
