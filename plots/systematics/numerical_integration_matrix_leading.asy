import root;
import pad_layout;
import common_code;

string topDir = "../../";

//string datasets[] = { "DS2a", "DS2b" };
string datasets[] = { "DS2b" };

string diagonal = "combined";

string objects[] = {
//	"g_eff_45b",
//	"g_eff_45t",
	"g_eff_comb1",
	"g_eff_comb2",
};

string object_labels[] = {
//	"45b-56t",
//	"45t-56b",
	"dgn. combination",
	"2nd dgn. combination",
};

string binning = "ob-1-4";

xSizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

int ci = -1;

void PlotMode(string label, string f, string tag, string obj = "g_eff_comb1")
{
	string pth = "contributions/" + tag + "/" + obj;
	RootObject obj = RootGetObject(f, pth, error=true);
	pen p = StdPen(++ci);
	draw(scale(1, 100), obj, "l,d0", p, label);
}

//----------------------------------------------------------------------------------------------------

void PlotAllModes(string f)
{
	ci = -1;

	AddToLegend("<{\it alignment}:");
	PlotMode("horizontal", f, "alig-sh-thx");
	PlotMode("vertical, right arm", f, "alig-sh-thy:D+0,R+1");
	PlotMode("vertical, right-left transfer", f, "alig-sh-thy:D+1,R+0");
	
	AddToLegend("<{\it alignment + optics}:");
	PlotMode("tilt in $x$-$y$ plane", f, "thx-thy-tilt");

	AddToLegend("<{\it optics}:");
	PlotMode("eigen mode 1", f, "opt-scale-m1");
	PlotMode("eigen mode 2", f,	"opt-scale-m2");
	
	AddToLegend("<{\it acceptance correction}:");
	PlotMode("beam divergence~RMS uncertainty", f, "acc-corr-sigma-unc");
	PlotMode("beam divergence left-right asymmetry", f, "acc-corr-sigma-asym");
	PlotMode("beam divergence non-gaussianity", f, "acc-corr-non-gauss");

	AddToLegend("<{\it uncorrelated 1-RP efficiencies}:");
	PlotMode("slope uncertainty, 45 bot.~-- 56 top", f, "eff-slp", "g_eff_comb1");
	PlotMode("slope uncertainty, 45 top -- 56 bot.", f, "eff-slp", "g_eff_comb2");

	AddToLegend("<{\it beam momentum}:");
	PlotMode("offset from nominal", f, "beam-mom");

	AddToLegend("<{\it unfolding}:");
	PlotMode("$\th_x^*$ resolution uncertainty", f, "unsm-sigma-x");
	PlotMode("$\th_y^*$ resolution uncertainty", f, "unsm-sigma-y");
	PlotMode("model dependence", f, "unsm-model");

	AddToLegend("<{\bf envelope of uncertainties}:");
	draw(scale(1, +100), RootGetObject(f, "matrices/all-anal/"+diagonal+"/"+binning+"/h_stddev"), "hl", black+1pt, "$\pm 1\un{\si}$");
	draw(scale(1, -100), RootGetObject(f, "matrices/all-anal/"+diagonal+"/"+binning+"/h_stddev"), "hl", black+1pt);
}


//----------------------------------------------------------------------------------------------------

frame f_legend;

int gy = 0;

for (int dsi : datasets.keys)
{
	++gy;

	string f = topDir + "systematics/"+datasets[dsi]+"/matrix_numerical_integration.root";
	
	NewPad("$|t|\ung{GeV^2}$", "relative cross-section variation$\ung{\%}$", 0, gy);
	currentpad.xTicks = LeftTicks(0.05, 0.01);
	PlotAllModes(f);
	limits((0, -1.5), (0.20, +1.5), Crop);
	f_legend = BuildLegend(3);
	currentpicture.legend.delete();
	AttachLegend("full $|t|$ range", N, N);

	NewPad("$|t|\ung{GeV^2}$", "relative cross-section variation$\ung{\%}$", 1, gy);
	currentpad.xTicks = LeftTicks(0.002, 0.001);
	PlotAllModes(f);
	limits((0, -7), (0.01, +2), Crop);
	currentpicture.legend.delete();
	AttachLegend("low $|t|$ zoom", N, N);


	/*
	//--------------------
	++gy;
	
	NewPad("$|t|\ung{GeV^2}$", "$|t|\ung{GeV^2}$", axesAbove=true, 0, gy);
	currentpad.xTicks = LeftTicks(0.05, 0.01);
	currentpad.yTicks = RightTicks(0.05, 0.01);
	TH2_z_min = -1;
	TH2_z_max = +1;
	draw(RootGetObject(f, "matrices/all-anal/"+diagonal+"/"+binning+"/h_corr_mat"), "p,bar");
	limits((0, 0), (0.20, 0.20), Crop);
	//AttachLegend("analysis uncertainties -- correlation matrix", SE, NE);
	*/

	//--------------------

	NewPad(false);
	attach(f_legend);
	FixPad(405, +220);
}

GShipout();
