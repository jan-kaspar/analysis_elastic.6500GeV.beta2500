import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5321"); dataset_fills.push("5321");

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

TGraph_errorBar = None;

xTicksDef = LeftTicks(1., 0.5);
yTicksDef = RightTicks(0.005, 0.001);

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 6cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------

for (int di : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[di] + "}");

	NewPad("time$\ung{h}$", "DAQ efficiency");
	real y_min = 0.985, y_max = 1.00;

	DrawRunBands(dataset_fills[di], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		string f = topDir+datasets[di]+"/daq_efficiency_" + diagonals[dgni] + ".root";
		pen p = StdPen(dgni+1);
		draw(swToHours, RootGetObject(f, "g_eff_vs_time"), "p", p, mCi+2pt+p, diagonal_labels[dgni]);
	}

	ylimits(y_min, y_max, Crop);

	SetPadWidth();
}

frame f_legend = BuildLegend();

NewPad(false);
add(f_legend);

GShipout(vSkip=1mm);
