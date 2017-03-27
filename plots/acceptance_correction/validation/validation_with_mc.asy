import root;
import pad_layout;

string topDir = "../../../acceptance/";

//string base = topDir + "simulations/0.013,1E7,new,curved";
string base = topDir + "simulations/1.100,1E7,new,curved";

string cols[] = {
	"simulations/1.100,1E7,new,curved",
	"simulations/0.013,1E7,new,curved",
};

string rows[] = {
	"seed1/validation_with_mc.root",
	"seed2/validation_with_mc.root",
	"seed3/validation_with_mc.root",
};

//----------------------------------------------------------------------------------------------------

for (string row : rows)
{
	NewRow();

	NewPad(false);
	label(replace(row, "_", "\_"));

	//--------------------

	string col = "simulations/1.100,1E8,new,curved";
	string f = topDir + col + "/" + row;
	string binning = "ob-1-20-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	scale(Linear, Log);
	draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue);
	draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen);
	draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red);
	limits((0, 1e-2), (1, 1e4), Crop);
	
	//--------------------
	
	string col = "simulations/0.013,1E8,new,curved";
	string f = topDir + col + "/" + row;
	string binning = "ob-1-20-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	currentpad.yTicks = RightTicks(5e3, 1e3);
	
	draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue, "el.~simu.");
	draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen, "el.~simu.~+ smearing + reco., full acceptance");
	draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red, "el.~simu.~+ smearing + reco., realistic acceptance");
	//xlimits(0, 0.01, Crop);
	limits((0, 4e4), (0.01, 8e4), Crop);
	
	yaxis(XEquals(8e-4, false), dashed);
	
	AttachLegend(NW, NE);
}
