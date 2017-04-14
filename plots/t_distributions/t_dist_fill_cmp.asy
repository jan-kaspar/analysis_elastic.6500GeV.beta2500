import root;
import pad_layout;

string topDir = "../../";

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5321"); dataset_fills.push("5321");

string diagonals[] = { "45b_56t", "45t_56b" };
string diagLabels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string binning = "ob-2-10-0.05";

drawGridDef = true;

xSizeDef = 8cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	xTicksDef = LeftTicks(0.2, 0.1);

	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$");
	scale(Linear, Log);

	for (int dgni : diagonals.keys)
	{
		pen p = StdPen(dgni + 1);
		draw(RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "normalization+unfolding/"+binning+"/h_t_normalized_unsmeared"),
			"d0,eb", p, diagLabels[dgni]);
	}

	limits((0, 1e-3), (1.0, 1e3), Crop);

	AttachLegend(datasets[dsi]);
}

//----------------------------------------------------------------------------------------------------

NewRow();

for (int dsi : datasets.keys)
{
	xTicksDef = LeftTicks(0.002, 0.001);

	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.~u.}$");
	//scale(Linear, Log);

	for (int dgni : diagonals.keys)
	{
		pen p = StdPen(dgni + 1);
		draw(RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "normalization/"+binning+"/h_t_normalized"),
			"d0,eb", p, diagLabels[dgni]);
	}

	limits((0, 400), (0.01, 1000), Crop);

	yaxis(XEquals(1.2e-3, false), dashed);

	AttachLegend(datasets[dsi]);
}
