import root;
import pad_layout;
include "../run_info.asy";

string datasets[], fills[];
datasets.push("DS-fill5317"); fills.push("5317");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };
pen dgn_pens[] = { blue, red };

string topDir = "../../";

xSizeDef = 10cm;
ySizeDef = 6cm;

//xTicksDef = LeftTicks(1., 0.5);

int rebin = 20;

//----------------------------------------------------------------------------------------------------

void DrawAll()
{
for (int dsi : datasets.keys)
{
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

	//ylimits(y_min, y_max, Crop);
	//SetPadWidth();
}
}

//----------------------------------------------------------------------------------------------------

void PlotLimit(real l)
{
	real x = l / 3600;
	yaxis(XEquals(x, false), black+1pt);
}

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int dgni : diagonals.keys)
{
	AddToLegend(dgn_labels[dgni], dgn_pens[dgni]);
}
AttachLegend();

//----------------------------------------------------------------------------------------------------

TH1_x_min = 100e3;
TH1_x_max = 120e3;

DrawAll();

limits((31, 0), (33, 40), Crop);

PlotLimit(112.1e3);
PlotLimit(118.4e3);

//----------------------------------------------------------------------------------------------------

TH1_x_min = 130e3;
TH1_x_max = 140e3;

DrawAll();

limits((36.5, 0), (37.5, 40), Crop);

PlotLimit(132.92e3);
PlotLimit(134.5e3);
