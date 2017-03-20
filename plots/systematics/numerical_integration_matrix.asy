import root;
import pad_layout;
import common_code;

AddAllModes();

string topDir = "../../";

string datasets[] = { "DS-fill5313" };

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

string binning = "ob-1-20-0.05";

//----------------------------------------------------------------------------------------------------

void PlotAllModes(string f)
{
	int ci = 0;

	for (int mi : modes.keys)
	{
		for (int oi : objects.keys)
		{
			string pth = "contributions/" + modes[mi].num_int_tag + "/" + objects[oi];
			RootObject obj = RootGetObject(f, pth, error=false);
			if (obj.valid)
			{
				draw(scale(1, 100), obj, "l,d0", StdPen(ci+1), modes[mi].label + " ("+ replace(object_labels[oi], "_", "\_") + ")");
				++ci;
			}
		}
	}

	// TODO
	//draw(scale(1, +100), RootGetObject(f, "matrices/all-anal/"+diagonal+"/"+binning+"/h_stddev"), "hl", black+1pt, "$\pm 1\un{\si}$ {\it envelope of analysis uncertainties}");
	//draw(scale(1, -100), RootGetObject(f, "matrices/all-anal/"+diagonal+"/"+binning+"/h_stddev"), "hl", black+1pt);
}


//----------------------------------------------------------------------------------------------------

frame f_legend;

int gy = 0;

for (int dsi : datasets.keys)
{
	++gy;

	string f = topDir + "systematics/"+datasets[dsi]+"/matrix_numerical_integration.root";
	
	NewPad(false, -1, gy);
	label("{\SetFontSizesXX "+replace(datasets[dsi], "_", "\_")+"}");

	NewPad("$|t|\ung{GeV^2}$", "systematic effect$\ung{\%}$", 0, gy);
	PlotAllModes(f);
	limits((0, -2), (0.20, +2), Crop);
	f_legend = BuildLegend();
	currentpicture.legend.delete();
	AttachLegend("full range");

	NewPad("$|t|\ung{GeV^2}$", "systematic effect$\ung{\%}$", 1, gy);
	PlotAllModes(f);
	limits((0, -5), (0.005, +2), Crop);
	yaxis(XEquals(8e-4, false), dashed, above=true);
	currentpad.xTicks = LeftTicks(0.001, 0.0005);
	currentpad.yTicks = RightTicks(1., 0.5);
	f_legend = BuildLegend();
	currentpicture.legend.delete();
	AttachLegend("low $|t|$ zoom");


	//--------------------

	// TODO
	/*
	++gy;
	
	NewPad("$|t|\ung{GeV^2}$", "$|t|\ung{GeV^2}$", axesAbove=true, 0, gy);
	currentpad.xTicks = LeftTicks(0.05, 0.01);
	currentpad.yTicks = RightTicks(0.05, 0.01);
	TH2_z_min = -1;
	TH2_z_max = +1;
	draw(RootGetObject(f, "matrices/all-anal/"+diagonal+"/"+binning+"/h_corr_mat"), "p,bar");
	limits((0, 0), (0.20, 0.20), Crop);
	AttachLegend("analysis uncertainties -- correlation matrix", SE, NE);
	*/

	//--------------------

	NewPad(false, 2, gy);
	//add(shift(0, 345) * f_legend);
	add(shift(0, +100) * f_legend);
}

GShipout();
