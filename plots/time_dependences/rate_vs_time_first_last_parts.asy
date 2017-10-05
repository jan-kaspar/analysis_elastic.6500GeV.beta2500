import root;
import pad_layout;
include "../run_info.asy";

string datasets[];
pen dataset_pens[];
datasets.push("DS-firstParts"); dataset_pens.push(blue);
datasets.push("DS-lastParts"); dataset_pens.push(red);

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };

string topDir = "../../";

xSizeDef = 10cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

int rebin = 100;

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 6cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int dsi : datasets.keys)
{
	AddToLegend(datasets[dsi], dataset_pens[dsi]);
}
AttachLegend();

//----------------------------------------------------------------------------------------------------

NewPad("time $\ung{h}$", "rate of el.~candidates$\ung{Hz}$");
//currentpad.yTicks = RightTicks(1., 0.2);
real y_min = 0, y_max = 40;

DrawRunBands(y_min, y_max);

for (int dsi : datasets.keys)
{
	for (int dgni : diagonals.keys)
	{
		RootObject hist = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "metadata/rate cmp|h_timestamp_sel");
		hist.vExec("Rebin", rebin);

		draw(scale(1., 1./rebin) * swToHours, hist, "vl", dataset_pens[dsi], dgn_labels[dgni]);	
	}
}

ylimits(y_min, y_max, Crop);
SetPadWidth();
