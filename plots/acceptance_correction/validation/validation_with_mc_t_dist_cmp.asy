import root;
import pad_layout;
include "../../common.asy";

string topDir = "../../../acceptance/";

string rows[] = {
//	"simulations/<t_max>,1E7,uncorr,old,flat/seed1/validation_with_mc.root",
//	"simulations/<t_max>,1E7,uncorr,new,flat/seed1/validation_with_mc.root",
	"simulations/<t_max>,1E7,uncorr,new,curved/seed1/validation_with_mc.root",

//	"simulations/<t_max>,1E7,corr,old,flat/seed1/validation_with_mc.root",
//	"simulations/<t_max>,1E7,corr,new,flat/seed1/validation_with_mc.root",
	"simulations/<t_max>,1E7,corr,new,curved/seed1/validation_with_mc.root",
};

//----------------------------------------------------------------------------------------------------

void PlotAll(string f, string binning)
{
	draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue, "el.~simu.");
	draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen, "el.~simu.~+ smearing");
	draw(RootGetObject(f, binning + "/h_t_re_acc_phi"), "eb", black, "el.~simu.~+ smearing + fid.~cut (phi) + acc.~corr (phi)");
	draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red, "el.~simu.~+ smearing + fid.~cut (all) + acc.~corr (all)");
}

//----------------------------------------------------------------------------------------------------

for (string row : rows)
{
	NewRow();

	NewPad(false);
	string lab = replace(row, "_", "\_");
	lab = replace(lab, "<t\_max>", "$\langle t_{\rm max}\rangle$");
	label(lab);

	//--------------------

	string f = topDir + replace(row, "<t_max>", "1.1");
	string binning = "ob-1-20-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	scale(Linear, Log);
	//currentpad.yTicks = RightTicks(100., 20.);
	PlotAll(f, binning);
	limits((0, 1e-2), (1.0, 1e4), Crop);

	//--------------------

	string f = topDir + replace(row, "<t_max>", "0.08");
	string binning = "ob-1-20-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	scale(Linear, Log);
	//currentpad.yTicks = RightTicks(100., 20.);
	PlotAll(f, binning);
	limits((0, 1e2), (0.075, 1e3), Crop);
	
	//--------------------
	
	string f = topDir + replace(row, "<t_max>", "0.006");
	string binning = "ob-1-20-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	currentpad.yTicks = RightTicks(100., 20.);
	PlotAll(f, binning);
	limits((0, 5e2), (0.004, 1e3), Crop);
	
	yaxis(XEquals(t_min, false), dashed);
	
	AttachLegend(NW, NE);
}
