struct Mode
{
	string label;
	pen p;

	string mc_file;
	string mc_tag;
	real mc_ref;

	string ni_file;
	string ni_tag;
	real ni_ref;
};

//----------------------------------------------------------------------------------------------------

Mode[] modes;

//----------------------------------------------------------------------------------------------------

void AddMode(string _l, pen _p = black,
	string _mcf, string _mct, real _mcr,
	string _nif, string _nit, real _nir
	)
{
	Mode m;
	
	m.label = _l;
	m.p = _p;

	m.mc_file = _mcf;
	m.mc_tag = _mct;
	m.mc_ref = _mcr;
	
	m.ni_file = _nif;
	m.ni_tag = _nit;
	m.ni_ref = _nir;

	modes.push(m);
}

//----------------------------------------------------------------------------------------------------

void AddAllModes()
{
	AddMode("\vbox{\hbox{shift in $\th_x^*$}\hbox{L-R symmetric}}", black,
		"", "sh-thx", 1,
		"", "sh-thx", 1
	);

	AddMode("\vbox{\hbox{shift in $\th_x^*$}\hbox{L-R anti-symmetric}}", black+dashed,
		"", "sh-thx-LRasym", 1,
		"", "sh-thx-LRasym", 1
	);

	//--------------------

	AddMode("\vbox{\hbox{shift in $\th_y^*$}\hbox{L-R symmetric}\hbox{T-B correlated}}", red,
		"", "sh-thy", 1,
		"", "sh-thy", 1
	);

	AddMode("\vbox{\hbox{shift in $\th_y^*$}\hbox{L-R anti-symmetric}\hbox{T-B correlated}}", red+dashed,
		"", "sh-thy-LRasym", 1,
		"", "sh-thy-LRasym", 1
	);

	AddMode("\vbox{\hbox{shift in $\th_y^*$}\hbox{L-R symmetric}\hbox{T-B uncorrelated}}", red,
		"", "sh-thy-TBuncor", 1,
		"", "sh-thy-TBuncor", 1
	);

	AddMode("\vbox{\hbox{shift in $\th_y^*$}\hbox{L-R anti-symmetric}\hbox{T-B uncorrelated}}", red,
		"", "sh-thy-TBuncor-LRasym", 1,
		"", "sh-thy-TBuncor-LRasym", 1
	);

	//--------------------

	AddMode("\vbox{\hbox{$x$-$y$ tilt}\hbox{L-R symmetric}}", darkred,
		"", "tilt-thx-thy", 1,
		"", "tilt-thx-thy", 1
	);

	AddMode("\vbox{\hbox{$x$-$y$ tilt}\hbox{L-R anti-symmetric}}", darkred+dashed,
		"", "tilt-thx-thy-LRasym", 1,
		"", "", 1
	);

	//--------------------

	AddMode("\vbox{\hbox{$\th_{x,y}^*$ scaling}\hbox{mode 1}}", heavygreen,
		"", "sc-thxy-mode1", 1,
		"", "sc-thxy-mode1", 1
	);

	AddMode("\vbox{\hbox{$\th_{x,y}^*$ scaling}\hbox{mode 2}}", heavygreen,
		"", "sc-thxy-mode2", 1,
		"", "sc-thxy-mode2", 1
	);

	AddMode("\vbox{\hbox{$\th_{x,y}^*$ scaling}\hbox{mode 3}}", heavygreen,
		"", "sc-thxy-mode3", 1,
		"", "sc-thxy-mode3", 1
	);
	
	//--------------------
	
	AddMode("\vbox{\hbox{uncert.~of $\si(d_x)$}}", blue+dashed,
		"", "dx-sigma", 1,
		"", "dx-sigma", 1
	);

	AddMode("\vbox{\hbox{uncert.~of $\si(d_y)$}}", red+dashed,
		"", "dy-sigma", 1,
		"", "dy-sigma", 1
	);
	
	AddMode("\vbox{\hbox{non-gaussianity of $d_x$}}", blue+dashed,
		"", "dx-non-gauss", 1,
		"", "dx-non-gauss", 1
	);

	AddMode("\vbox{\hbox{correlation $d_x$ vs.~$m_x$}}", red+dashed,
		"", "dx-mx-corr", 1,
		"", "dx-mx-corr", 1
	);

	AddMode("\vbox{\hbox{correlation $d_y$ vs.~$m_y$}}", red+dashed,
		"", "dy-my-corr", 1,
		"", "dy-my-corr", 1
	);

	//--------------------

	AddMode("\vbox{\hbox{inefficiency correction:}\hbox{intercept uncertainty}}", black+dashed,
		"", "eff-intercept", 1,
		"", "eff-intercept", 1
	);
	
	AddMode("\vbox{\hbox{inefficiency correction:}\hbox{slope uncertainty}}", black+dashed,
		"", "eff-slope", 1,
		"", "eff-slope", 1
	);

	//--------------------
	
	AddMode("\vbox{\hbox{beam momentum}}", magenta,
		"", "beam-mom", 1,
		"", "beam-mom", 1
	);

	//--------------------

	AddMode("\vbox{\hbox{unsmearing:}\hbox{uncert.~of $\si(m_x)$}}", blue+dashed,
		"", "mx-sigma", 1,
		"", "mx-sigma", 1
	);

	AddMode("\vbox{\hbox{unsmearing:}\hbox{uncert.~of $\si(m_y)$}}", blue+dashed,
		"", "my-sigma", 1,
		"", "my-sigma", 1
	);

	AddMode("\vbox{\hbox{unsmearing:}\hbox{model dependence}}", blue+dashed,
		"", "unsmearing-model", 1,
		"", "unsmearing-model", 1
	);

	//--------------------

	AddMode("\vbox{\hbox{normalisation}}", magenta+dashed,
		"", "norm", 1,
		"", "norm", 1
	);
}

//----------------------------------------------------------------------------------------------------

void FilterModes(... string filters[])
{
	Mode sel[];

	for (int mi : modes.keys)
	{
		bool keep = false;
		for (string f : filters)
		{
			if (find(modes[mi].mc_tag, f) >= 0 || find(modes[mi].ni_tag, f) >= 0)
			{
				keep = true;
				break;
			}
		}
	
		if (keep)
			sel.push(modes[mi]);
	}

	modes = sel;
}
