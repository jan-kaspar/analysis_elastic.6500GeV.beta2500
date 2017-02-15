import root;
import pad_layout;

string topDir = "../../";

string binnings[] = {
	"ob-1-20-0.05",
	"ob-2-10-0.05",
	"ob-3-5-0.05",
};

drawGridDef = true;

xSizeDef = 12cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

for (int bi : binnings.keys)
{
	NewRow();

	string binning = binnings[bi];

	// ----------

	NewPad("$|t|\ung{GeV^2}$", "$\d N/\d t\ung{a.~u.}$");
	scale(Linear, Log);
	currentpad.xTicks = LeftTicks(0.2, 0.1);

	draw(RootGetObject(topDir+"DS-merged/merged.root", binning + "/merged/combined/h_dsdt"), "d0,eb", red);

	limits((0, 1e3), (1., 1e9), Crop);
	
	// ----------

	NewPad("$|t|\ung{GeV^2}$", "$\d N/\d t\ung{a.~u.}$");
	currentpad.xTicks = LeftTicks(0.002, 0.001);

	draw(RootGetObject(topDir+"DS-merged/merged.root", binning + "/merged/combined/h_dsdt"), "d0,eb", red);

	limits((0, 2e8), (0.02, 5e8), Crop);

}
