import root;
import pad_layout;

string datasets[];
datasets.push("DS-fill5313");
datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
datasets.push("DS-fill5321");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };

string topDir = "../../";

string histogram = "acceptance correction/ob-3-5-0.05/h_t_after";

string combinations[];
pen comb_pens[];

//combinations.push("n_si=2"); comb_pens.push(black);
combinations.push("n_si=3"); comb_pens.push(red);
combinations.push("n_si=4"); comb_pens.push(blue);
combinations.push("n_si=5"); comb_pens.push(heavygreen);
//combinations.push("n_si=6"); comb_pens.push(magenta);

string ref_comb = "n_si=4";

xSizeDef = 10cm;

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void PlotRatio(RootObject o, RootObject r, pen p)
{
	int N = r.iExec("GetNbinsX");
	for (int i = 1; i <= N; ++i)
	{
		real c = r.rExec("GetBinCenter", i);
		real w = r.rExec("GetBinWidth", i);

		real v = o.rExec("GetBinContent", i);
		real u = o.rExec("GetBinError", i);

		real v_r = r.rExec("GetBinContent", i);
		real u_r = r.rExec("GetBinError", i);

		real ratio = (v_r != 0) ? (v / v_r - 1.) * 100.: 0.;
		//real ratio_unc = (u > 0. && u_r > 0.) ? v / v_r * 100 * sqrt( (u/v)^2 + (u_r/v_r)^2 ) : 0.;
		real ratio_unc = 0.;

		draw(Scale((c, ratio)), mCi+p);
		draw(Scale((c, ratio-ratio_unc))--Scale((c, ratio+ratio_unc)), p);
		draw(Scale((c-w/2, ratio))--Scale((c+w/2, ratio)), p);
	}
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgi : diagonals.keys)
	{
		NewRow();

		string dataset = datasets[dsi];
		string diagonal = diagonals[dgi];

		NewPad(false);
		label(replace("{\SetFontSizesXX\vbox{\hbox{"+dataset+"}\hbox{"+dgn_labels[dgi]+"}}}", "_", "\_"));

		//--------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t\ung{mb/GeV^2}$");
		scale(Linear, Log);
		for (int ci : combinations.keys)
		{
			string label = replace(combinations[ci], "_", "\_");
			if (combinations[ci] == ref_comb)
				label += " (reference)";
		
			string f = topDir+"background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			draw(RootGetObject(f, histogram), "eb", comb_pens[ci], label);
		}
		
		frame fLegend = BuildLegend();
		
		limits((0, 1e2), (1.0, 1e8), Crop);

		//--------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t\ung{mb/GeV^2}$");
		scale(Linear, Log);
		currentpad.xTicks = LeftTicks(0.05, 0.01);

		for (int ci : combinations.keys)
		{
			string label = replace(combinations[ci], "_", "\_");
			if (combinations[ci] == ref_comb)
				label += " (reference)";
		
			string f = topDir+"background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			draw(RootGetObject(f, histogram), "eb", comb_pens[ci], label);
		}
		
		frame fLegend = BuildLegend();
		
		limits((0, 1e6), (0.2, 1e8), Crop);

		//--------------------
		
		NewPad(false);
		attach(fLegend);
		
		//--------------------
		
		string ref_f = topDir+"background_studies/"+dataset+"/"+ref_comb+"/distributions_"+diagonal+".root";
		RootObject ref_o = RootGetObject(ref_f, histogram);
		
		//--------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t$: (test - ref) / ref$\ung{\%}$");
		for (int ci : combinations.keys)
		{
			//if (combinations[ci] == ref_comb)
			//	continue;
		
			string f = topDir+"background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			RootObject o = RootGetObject(f, histogram);
			PlotRatio(o, ref_o, comb_pens[ci]);
		}
		
		limits((0, -20), (1.0, 20), Crop);
		
		//--------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t$: (test - ref) / ref$\ung{\%}$");
		for (int ci : combinations.keys)
		{
			//if (combinations[ci] == ref_comb)
			//	continue;
		
			string f = topDir+"background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			RootObject o = RootGetObject(f, histogram);
			PlotRatio(o, ref_o, comb_pens[ci]);
		}
		
		limits((0, -1), (0.3, 1), Crop);
	}
}
