import root;
import pad_layout;

string f = "validation_with_mc.root";

//----------------------------------------------------------------------------------------------------

string binning = "ub";

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
scale(Linear, Log);
draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue);
draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen);
draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red);
xlimits(0, 1, Crop);

//----------------------------------------------------------------------------------------------------

string binning = "ob-1-30-0.05";

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
//scale(Linear, Log);
draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue, "simulation truth");
draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen, "reconstruction, full acceptance");
draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red, "reconstruction, realistic acceptance");
xlimits(0, 0.01, Crop);

yaxis(XEquals(8e-4, false), dashed);

AttachLegend(NW, NE);
