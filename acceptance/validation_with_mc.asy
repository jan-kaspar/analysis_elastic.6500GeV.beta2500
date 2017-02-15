import root;
import pad_layout;

//string files[] = {
//	"validation_with_mc_problem.root"
//};

string base = "simulations/0.013,2E8,new,curved";

string files[] = {
	base + "/seed1/validation_with_mc.root",
	base + "/seed2/validation_with_mc.root",
	base + "/seed3/validation_with_mc.root",
};

//----------------------------------------------------------------------------------------------------

for (string f : files)
{
	NewRow();

	NewPad(false);
	label(replace(f, "_", "\_"));

	//--------------------
	
	RootObject obj = RootGetObject(f, "h_th_x_th_y_true", error=false);
	if (!obj.valid)
		continue;

	//--------------------

	string binning = "ub";
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	scale(Linear, Log);
	draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue);
	draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen);
	draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red);
	xlimits(0, 1, Crop);
	
	//--------------------
	
	string binning = "ob-1-30-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	//scale(Linear, Log);
	draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue, "simulation truth");
	draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen, "reconstruction, full acceptance");
	draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red, "reconstruction, realistic acceptance");
	//xlimits(0, 0.01, Crop);
	limits((0, 60), (0.01, 80), Crop);
	
	yaxis(XEquals(8e-4, false), dashed);
	
	AttachLegend(NW, NE);
}
