import root;
import pad_layout;

string topDir = "../../";

string datasets[];
pen dataset_pens[];
datasets.push("DS-firstParts"); dataset_pens.push(blue);
datasets.push("DS-lastParts"); dataset_pens.push(red);

drawGridDef = true;

TH1_x_min = 8.1e-4;

string binning = "ob-2-10-0.05";

string diagonal = "combined";

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$", 12cm, 9cm);
scale(Linear, Log);

for (int dsi : datasets.keys)
{
	pen p = dataset_pens[dsi];

	draw(RootGetObject(topDir+"DS-merged/merged.root", binning + "/" + datasets[dsi] + "/" + diagonal + "/h_dsdt"),
		"d0,eb,vl", p, datasets[dsi]);
}

currentpad.xTicks = LeftTicks(0.2, 0.1);
limits((0, 1e-2), (1, 1e3), Crop);

AttachLegend();

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$", 12cm, 9cm);

for (int dsi : datasets.keys)
{
	pen p = dataset_pens[dsi];

	draw(RootGetObject(topDir+"DS-merged/merged.root", binning + "/" + datasets[dsi] + "/" + diagonal + "/h_dsdt"),
		"d0,eb,vl", p, datasets[dsi]);
}

currentpad.xTicks = LeftTicks(0.002, 0.001);
currentpad.yTicks = RightTicks(100., 50.);
limits((0, 400), (0.01, 1000), Crop);

yaxis(XEquals(8e-4, false), dashed);

AttachLegend();
