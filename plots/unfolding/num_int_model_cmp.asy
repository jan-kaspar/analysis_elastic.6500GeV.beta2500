import root;
import pad_layout;

string topDir = "../../";

string diagonals[] = { "45t_56b" };
string diagonal_labels[] = { "45t -- 56b" };

string models[] = {
	"fit2-1",
	"fit2-2",
	"fit2-4",
};

string dataset = "DS-fill5313";

xSizeDef = 10cm;
ySizeDef = 8cm;

TGraph_x_min = 1.2e-3;

//----------------------------------------------------------------------------------------------------

for (int dgni : diagonals.keys)
{
	NewRow();

	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + dataset + "}\hbox{" + diagonal_labels[dgni] + "}}");


	// ----- full t range -----

	NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction");
	currentpad.yTicks = RightTicks(0.005, 0.001);

	for (int mi : models.keys)
	{
		string f = topDir + dataset + "/unfolding_cf_ni_" + diagonals[dgni] + ".root";
		draw(RootGetObject(f, models[mi] + "/g_t_corr"), StdPen(mi), replace(models[mi], "_", "\_"));
	}

	limits((0, 0.995), (1., 1.005), Crop);

	AttachLegend(SE, SE);


	// ----- low |t| zoom -----

	NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction");
	currentpad.xTicks = LeftTicks(0.0005, 0.0001);
	currentpad.yTicks = RightTicks(0.005, 0.001);

	for (int mi : models.keys)
	{
		string f = topDir + dataset + "/unfolding_cf_ni_" + diagonals[dgni] + ".root";
		draw(RootGetObject(f, models[mi] + "/g_t_corr"), StdPen(mi), replace(models[mi], "_", "\_"));
	}

	limits((0.0005, 0.995), (0.003, 1.005), Crop);

	yaxis(XEquals(1.2e-3, false), dashed);

	AttachLegend(SE, SE);
}

GShipout(margin=1mm);
