import root;
import pad_layout;

string topDir = "../../";

string datasets[], dataset_fills[];
datasets.push("DS-simulation"); dataset_fills.push("simulation");

string diagonals[] = { "45b_56t", "45t_56b" };
string diagLabels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

drawGridDef = true;

xSizeDef = 8cm;
ySizeDef = 8cm;

real scale_adjust = 1.;

TH1_x_min = 8.1e-4;

//----------------------------------------------------------------------------------------------------

void PlotAll(string dataset, string diagonal, string binning)
{
	draw(RootGetObject(topDir+dataset+"/distill_"+diagonal+".root", "model/g_df"), "l", blue+1pt, "MC input");

	transform tr = (currentpicture.scale.y.scale.logarithmic) ? shift(0, log10(scale_adjust)) : scale(1., scale_adjust);

	draw(tr, RootGetObject(topDir+dataset+"/distributions_"+diagonal+".root",
		"normalization+unfolding/"+binning+"/h_t_normalized_unsmeared"), "d0,eb", red+1pt, "MC + analysis chain");
}

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgni : diagonals.keys)
	{
		NewRow();

		NewPad(false);
		label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}\hbox{" + diagLabels[dgni] + "}}");

		// --------------------

		xTicksDef = LeftTicks(0.2, 0.1);

		NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$");
		scale(Linear, Log);

		string binning = "ob-2-10-0.05";
		PlotAll(datasets[dsi], diagonals[dgni], binning);

		limits((0, 1e-3), (1.0, 1e3), Crop);

		AttachLegend(datasets[dsi]);

		// --------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.~u.}$");
		currentpad.xTicks = LeftTicks(0.002, 0.001);
		currentpad.yTicks = RightTicks(100., 50.);

		string binning = "ob-1-20-0.05";
		PlotAll(datasets[dsi], diagonals[dgni], binning);

		limits((0, 400), (0.01, 1000), Crop);

		yaxis(XEquals(8e-4, false), dashed);

		AttachLegend();
	}
}
