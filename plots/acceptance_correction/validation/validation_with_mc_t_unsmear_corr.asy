import root;
import pad_layout;

string topDir = "../../../";

string baseDir = topDir + "acceptance/";

string rows[] = {
//	"simulations/<t_max>,1E9,uncorr,old,flat/seed1/validation_with_mc.root",
//	"simulations/<t_max>,1E9,uncorr,new,flat/seed1/validation_with_mc.root",
	"simulations/<t_max>,1E9,uncorr,new,curved/seed1/validation_with_mc.root",

//	"simulations/<t_max>,1E9,corr,old,flat/seed1/validation_with_mc.root",
//	"simulations/<t_max>,1E9,corr,new,flat/seed1/validation_with_mc.root",
	"simulations/<t_max>,1E9,corr,new,curved/seed1/validation_with_mc.root",
};

string ref_file = topDir + "DS-fill5313/unfolding_cf_ni_45b_56t.root";
string ref_obj = "fit2-2/g_t_corr";
TGraph_x_min = 8e-4;

//----------------------------------------------------------------------------------------------------

void PlotAll(string f, string binning)
{
	draw(RootGetObject(f, binning + "/h_t_ratio_tr_over_re_acc_phi"), "eb", blue, "fiducial cuts: phi only");
	draw(RootGetObject(f, binning + "/h_t_ratio_tr_over_re_acc"), "eb", red, "fiducial cuts: all");

	draw(RootGetObject(ref_file, ref_obj), "l", black, "dedicated calculation");
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
	
	string f = baseDir + replace(row, "<t_max>", "1.1");
	string binning = "ob-3-5-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "unsmearing correction (multiplicative)");
	currentpad.yTicks = RightTicks(0.002, 0.001);
	PlotAll(f, binning);	
	limits((0, 0.995), (1.0, 1.005), Crop);

	//--------------------
	
	string f = baseDir + replace(row, "<t_max>", "0.006");
	string binning = "ob-1-20-0.05";
	
	NewPad("$|t|\ung{GeV^2}$", "unsmearing correction (multiplicative)");
	currentpad.yTicks = RightTicks(0.002, 0.001);
	PlotAll(f, binning);	
	limits((0, 0.995), (0.005, 1.005), Crop);
	
	yaxis(XEquals(8e-4, false), dashed);
	
	AttachLegend(NW, NE);
}
