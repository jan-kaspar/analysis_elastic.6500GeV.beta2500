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

string binning = "ob-1-30-0.05";

drawGridDef = true;

xTicksDef = LeftTicks(0.2, 0.1);

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.~u.}$", 10cm, 8cm);
	scale(Linear, Log);

	for (int dgni : diagonals.keys)
	{
		pen p = StdPen(dgni + 1);
		draw(RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "acceptance correction/"+binning+"/h_t_after"),
			"d0,eb", p, diagLabels[dgni]);
	}

	limits((0, 1e1), (1.0, 1e9), Crop);
	//limits((0, 1e7), (0.1, 1e9), Crop);

	AttachLegend(datasets[dsi] + " -- fill " + dataset_fills[dsi]);
}
