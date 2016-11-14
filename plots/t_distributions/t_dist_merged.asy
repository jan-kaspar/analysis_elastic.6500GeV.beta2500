import root;
import pad_layout;

string topDir = "../../";

string diagonals[], diagLabels[];
diagonals.push("45b_56t"); diagLabels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagLabels.push("45 top -- 56 bot");

string binning = "ob-1-30-0.05";

drawGridDef = true;

bool lowT = false;

//----------------------------------------------------------------------------------------------------

//NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.~u.}$", 12cm, 9cm);
NewPad("$|t|\ung{GeV^2}$", "", 12cm, 9cm);

if (!lowT)
	scale(Linear, Log);

for (int dgni : diagonals.keys)
{
	pen p = StdPen(dgni + 1);

	draw(RootGetObject(topDir+"DS-merged/merged.root", binning + "/merged/" + diagonals[dgni] + "/h_dsdt"),
		"d0,eb,vl", p, diagLabels[dgni]);

	AddToLegend(format("events $%.2E$", robj.rExec("GetEntries")));
}

if (lowT)
{
	currentpad.xTicks = LeftTicks(0.002, 0.001);
	currentpad.yTicks = RightTicks(5e7, 1e7);
	limits((0, 1e7), (0.02, 3e8), Crop);
} else {
	currentpad.xTicks = LeftTicks(0.2, 0.1);
	limits((0, 1e3), (1.1, 1e9), Crop);
}

AttachLegend();
