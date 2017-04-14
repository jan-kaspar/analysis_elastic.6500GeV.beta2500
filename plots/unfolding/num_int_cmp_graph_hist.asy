import root;
import pad_layout;

string topDir = "../../";

string diagonals[] = { "45t_56b" };
string diagonal_labels[] = { "45t -- 56b" };

string model = "fit2-2";

string dataset = "DS-fill5313";

string binnings[] = {
	"ob-1-20-0.05",
	"ob-2-10-0.05",
	"ob-3-5-0.05",
};

xSizeDef = 10cm;
ySizeDef = 8cm;

TGraph_x_min = 6e-4;
TH1_x_min = 1.2e-3;

//----------------------------------------------------------------------------------------------------

void PlotAll(string f)
{
	draw(RootGetObject(f, model + "/g_t_corr"), black+1pt, "graph");

	for (int bi : binnings.keys)
	{
		draw(RootGetObject(f, model + "/" + binnings[bi]), "vl", StdPen(bi+1), binnings[bi]);
	}
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dgni : diagonals.keys)
{
	NewRow();

	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + dataset + "}\hbox{" + diagonal_labels[dgni] + "}}");

	string f = topDir + dataset + "/unfolding_cf_ni_" + diagonals[dgni] + ".root";

	// ----- full t range -----

	NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction");
	currentpad.yTicks = RightTicks(0.001, 0.0005);

	PlotAll(f);

	limits((0, 0.995), (1., 1.001), Crop);

	AttachLegend(SE, SE);


	// ----- low |t| zoom -----

	NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction");
	currentpad.xTicks = LeftTicks(0.0005, 0.0001);
	currentpad.yTicks = RightTicks(0.001, 0.0005);
	
	PlotAll(f);

	limits((0.0005, 0.995), (0.003, 1.001), Crop);

	yaxis(XEquals(1.2e-3, false), dashed);

	AttachLegend(SE, SE);
}

GShipout(margin=1mm);
