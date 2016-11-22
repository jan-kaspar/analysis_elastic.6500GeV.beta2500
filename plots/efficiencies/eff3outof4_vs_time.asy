import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5321"); dataset_fills.push("5321");

//string diagonals[] = { "45t_56b" };
string diagonals[] = { "45b_56t", "45t_56b" };

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("L-220-fr");
rps.push("L_1_F"); rp_labels.push("L-210-fr");
rps.push("R_1_F"); rp_labels.push("R-210-fr");
rps.push("R_2_F"); rp_labels.push("R-220-fr" );


yTicksDef = RightTicks(1., 0.2);
xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;


//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 3cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------

frame f_legend;

for (int rpi : rps.keys)
{
	NewRow();

	NewPad(drawAxes=false);
	label("{\SetFontSizesXX " + rp_labels[rpi] + "}");

	for (int dsi : datasets.keys)
	{
		NewPad("time $\ung{h}$", "efficiency\ung{\%}");
		//DrawRunBands(65, 105);

		for (int dgi : diagonals.keys)
		{
			string f = topDir + datasets[dsi] + "/eff3outof4_"+diagonals[dgi]+".root";
			string d = "excluded RPs " + rps[rpi] + "/n_si 3.0";

			pen p = StdPen(dgi+1);

			string label = replace(diagonals[dgi], "_", " -- ");

			draw(swToHours * scale(1, 100), RootGetObject(f, d+"/th_y dependence with th_x cut, per period/g_eff_vs_time"),
				"p", p, mCi+2pt+p, label);
		}

		ylimits(95., 100., Crop);

		SetPadWidth();
	}

	f_legend = BuildLegend();
}

NewPad(false);
add(f_legend);
