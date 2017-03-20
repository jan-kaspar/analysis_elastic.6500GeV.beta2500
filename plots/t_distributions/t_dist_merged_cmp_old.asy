import root;
import pad_layout;

string topDir = "../../";

string files[] = {
	"DS-merged/merged.root",
	"DS-merged/merged_old.root",
};

string binning = "ob-1-20-0.05";

drawGridDef = true;

xSizeDef = 12cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

void PlotAll()
{
	for (int fi : files.keys)
		draw(RootGetObject(topDir+files[fi], binning + "/merged/combined/h_dsdt"), "d0,eb", StdPen(fi), replace(files[fi], "_", "\_"));
}

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$");
scale(Linear, Log);
PlotAll();
currentpad.xTicks = LeftTicks(0.2, 0.1);
limits((0, 1e-3), (1, 1e3), Crop);

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$");
PlotAll();
currentpad.xTicks = LeftTicks(0.002, 0.001);
limits((0, 400), (0.01, 1000), Crop);

frame f_legend = BuildLegend();

NewPad(false);
add(f_legend);
