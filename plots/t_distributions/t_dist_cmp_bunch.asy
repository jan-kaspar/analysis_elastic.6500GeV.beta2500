import root;
import pad_layout;

string topDir = "../../";

string dataset = "DS-fill5317";

string bunches[];
bunches.push("10");
bunches.push("713");
bunches.push("1246");
bunches.push("1874");
bunches.push("2429");

string diagonals[], diagLabels[];
diagonals.push("45b_56t"); diagLabels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagLabels.push("45 top -- 56 bot");

drawGridDef = true;

TH1_x_min = 8.1e-4;

//----------------------------------------------------------------------------------------------------

for (int dgni : diagonals.keys)
{
	string diagonal = diagonals[dgni];

	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + diagLabels[dgni] + "}");

	NewPad("$|t|\ung{GeV^2}$", "$\d N/\d t\ung{mb/GeV^2}$", 12cm, 9cm);
	scale(Linear, Log);

	string binning = "ob-3-5-0.05";

	for (int bi : bunches.keys)
	{
		pen p = StdPen(bi + 1);

		string f = topDir+dataset + "/per-bunch/" + bunches[bi] + "/distributions_" + diagonal + ".root";
		string on = "normalization+unfolding/" + binning + "/h_t_normalized_unsmeared";
		draw(RootGetObject(f, on), "d0,eb,vl", p, bunches[bi]);
	}

	currentpad.xTicks = LeftTicks(0.2, 0.1);
	limits((0, 1e-3), (1.1, 1e3), Crop);

	//AttachLegend();

	//----------------------

	NewPad("$|t|\ung{GeV^2}$", "$\d N/\d t\ung{mb/GeV^2}$", 12cm, 9cm);

	AddToLegend("<bunch:");

	string binning = "ob-3-5-0.05";

	for (int bi : bunches.keys)
	{
		pen p = StdPen(bi + 1);

		string f = topDir+dataset + "/per-bunch/" + bunches[bi] + "/distributions_" + diagonal + ".root";
		string on = "normalization+unfolding/" + binning + "/h_t_normalized_unsmeared";
		draw(RootGetObject(f, on), "d0,eb,vl", p, bunches[bi]);
	}

	currentpad.xTicks = LeftTicks(0.002, 0.001);
	currentpad.yTicks = RightTicks(100., 50.);
	limits((0, 400), (0.01, 1000), Crop);

	yaxis(XEquals(8e-4, false), dashed);

	AttachLegend();
}
