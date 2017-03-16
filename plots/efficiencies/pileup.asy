import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
//datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5317/block1"); dataset_fills.push("5317.1");
datasets.push("DS-fill5317/block2"); dataset_fills.push("5317.2");
datasets.push("DS-fill5321"); dataset_fills.push("5321");

TGraph_errorBar = None;

xTicksDef = LeftTicks(1., 0.5);
yTicksDef = RightTicks(0.01, 0.005);

bool showFits = false;

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

	NewPad("time$\ung{h}$", "destructive pile-up probability", 12cm);
	real y_min = 0, y_max = 0.10;

	DrawRunBands(dataset_fills[di], y_min, y_max);

	draw(swToHours, RootGetObject(topDir+datasets[di]+"/pileup_fit_combined.root", "45b_56t/dgn.src"), "p", blue, mTU+2pt+blue, "45 bot -- 56 top");

	if (showFits)
		draw(swToHours, RootGetObject(topDir+datasets[di]+"/pileup_fit_combined.root", "45b_56t/dgn"), "l", blue+dashed);
	
	draw(swToHours, RootGetObject(topDir+datasets[di]+"/pileup_fit_combined.root", "45t_56b/dgn.src"), "p", red, mTD+2pt+red, "45 top -- 56 bot");

	if (showFits)
		draw(swToHours, RootGetObject(topDir+datasets[di]+"/pileup_fit_combined.root", "45t_56b/dgn"), "l", red+dashed);

	SetPadWidth();

	ylimits(0, 0.1, Crop);
}

frame f_legend = BuildLegend();

NewPad(false);
add(f_legend);

GShipout(vSkip=1mm);
