import root;
import pad_layout;

string datasets[];
datasets.push("DS-fill5313");
datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
datasets.push("DS-fill5321");

string diagonals[] = { "45b_56t", "45t_56b" };

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("L-220-fr");
rps.push("L_1_F"); rp_labels.push("L-210-fr");
rps.push("R_1_F"); rp_labels.push("R-210-fr");
rps.push("R_2_F"); rp_labels.push("R-220-fr" );

string topDir = "../../";

xSizeDef = 6cm;
ySizeDef = 5cm;

//yTicksDef = RightTicks(5., 1.);

//----------------------------------------------------------------------------------------------------

NewPad(false);

for (int dgi : diagonals.keys)
{
	NewPad(false);
	label("{\SetFontSizesXX " + replace(diagonals[dgi], "_", "\_") + "}");

	NewPad(false);
}

for (int rpi : rps.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + rp_labels[rpi] + "}");

	for (int dgi : diagonals.keys)
	{
		pad p_a = NewPad("dataset", "slope$\ung{rad^{-1}}$");
		pad p_b = NewPad("dataset", "intercept$\ung{\%}$");

		// plot fits per dataset
		for (int dsi : datasets.keys)
		{
			if (rp_labels[rpi] == "L-220-fr" && diagonals[dgi] == "45t_56b" && datasets[dsi] == "DS-fill5317")
				continue;

			string f = topDir + datasets[dsi] + "/eff3outof4_fit.root";
			string d = diagonals[dgi] + "/" + rps[rpi];
			RootObject fit = RootGetObject(f, d+"/pol1");

			real a = fit.rExec("GetParameter", 1), a_unc = fit.rExec("GetParError", 1);
			real b = fit.rExec("GetParameter", 0)*100, b_unc = fit.rExec("GetParError", 0)*100;

			b_unc = 0.5;

			real x = dsi;

			SetPad(p_a);
			draw((x, a), mCi+red+2pt);
			draw((x, a-a_unc)--(x, a+a_unc), red);

			SetPad(p_b);
			draw((x, b), mCi+blue+2pt);
			draw((x, b-b_unc)--(x, b+b_unc), blue);
		}

		// get global fits	
		string f = topDir + "efficiency_fits/global_fit.root";
		string d = diagonals[dgi] + "/" + rps[rpi];
		RootObject fit_gl = RootGetObject(f, d+"/pol1");
		
		real a_g = fit_gl.rExec("GetParameter", 1), a_g_unc = fit_gl.rExec("GetParError", 1);
		real b_g = fit_gl.rExec("GetParameter", 0)*100, b_g_unc = fit_gl.rExec("GetParError", 0)*100;

		// finalise pads
		SetPad(p_a);
		currentpad.yTicks = RightTicks(50., 10.);
		string avg_label = format("avg = $%#.1f$", a_g) + format("$\pm %#.1f$", a_g_unc);
		draw(Label(avg_label, 0.5, S, Fill(white)), (-0.5, a_g)--(datasets.length-0.5, a_g), red);
		limits((-1, -200), (datasets.length, +0), Crop);

		SetPad(p_b);
		currentpad.yTicks = RightTicks(5., 1.);
		string avg_label = format("avg = $%#.1f$", b_g) + format("$\pm %#.1f$", b_g_unc);
		draw(Label(avg_label, 0.5, S, Fill(white)), (-0.5, b_g)--(datasets.length-0.5, b_g), blue);
		limits((-1, 94), (datasets.length, 100), Crop);
	}

}



GShipout(vSkip=0pt);
