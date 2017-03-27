import root;
import pad_layout;

string topDir = "../../";

string diagonals[] = { "45t_56b" };
string diagonal_labels[] = { "45t -- 56b" };

string model = "new_fit:2,KL,con-g_fit";

string extensions[] = {
	"x+1",
	"x-1",
	"y+1",
	"y-1",
};

xSizeDef = 10cm;
ySizeDef = 8cm;

TGraph_x_min = 6e-4;

//----------------------------------------------------------------------------------------------------

for (int dgni : diagonals.keys)
{
	string dir = topDir + "unsmearing_correction/numerical_integration/DS2b," + diagonals[dgni] + "/" + model;
	
	NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction");
	currentpad.xTicks = LeftTicks(0.0005, 0.0001);
	currentpad.yTicks = RightTicks(0.005, 0.001);

	draw(rGetObj(dir + "/numerical_integration.root", "unsm_corr"), black, "central");

	for (int ei : extensions.keys)
	{
		string f_ni = dir + "/numerical_integration_" + extensions[ei] + ".root";
		draw(rGetObj(f_ni, "unsm_corr"), StdPen(ei+1), replace(extensions[ei], "_", "\_"));
	}

	limits((0.0005, 0.97), (0.003, 1.01), Crop);

	for (real y = 0.97; y <= 1.01; y += 0.005)
		xaxis(YEquals(y, false), (fabs(y - 1.) < 1e-4) ? dashed : dotted);

	for (real x = 0.0005; x <= 0.003; x += 0.0005)
		yaxis(XEquals(x, false), dotted);

	yaxis(XEquals(6e-4, false), dashed);

	AttachLegend(diagonal_labels[dgni], SE, SE);
}


GShipout(margin=1mm);
