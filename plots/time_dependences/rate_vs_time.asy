import root;
import pad_layout;
include "../run_info.asy";

string datasets[], fills[];
datasets.push("DS-fill5313"); fills.push("5313");
datasets.push("DS-fill5314"); fills.push("5314");
datasets.push("DS-fill5317"); fills.push("5317");
datasets.push("DS-fill5321"); fills.push("5321");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };
pen dgn_pens[] = { blue, red };

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
for (int dgni : diagonals.keys)
{
	AddToLegend(dgn_labels[dgni], dgn_pens[dgni]);
}
AttachLegend();

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	NewPad("time $\ung{h}$", "rate of el.~candidates$\ung{Hz}$");
	//currentpad.yTicks = RightTicks(1., 0.2);
	real y_min = 0, y_max = 40;
	
	DrawRunBands(fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		RootObject hist = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "metadata/rate cmp|h_timestamp_sel");
		hist.vExec("Rebin", rebin);

		draw(scale(1., 1./rebin) * swToHours, hist, "vl", dgn_pens[dgni], dgn_labels[dgni]);	
	}

	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}
