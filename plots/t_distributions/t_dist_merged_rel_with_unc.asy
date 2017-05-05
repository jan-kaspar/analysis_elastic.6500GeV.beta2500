import root;
import pad_layout;
include common_code;

string topDir = "../../";

string dataset = "merged";

string diagonal = "combined";
string binning = "ob-3-5-0.05";


string fit_file = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/second_fit/do_fit.root";
string fit_obj = "variant 2/g_dsdt_CH";

string unc_file = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/2rp/systematics/DS-fill5313/matrix_numerical_integration.root";
string unc_types[], unc_labels[];
pen unc_pens[];
//unc_types.push("all"); unc_pens.push(blue+opacity(0.5)); unc_labels.push("all");
unc_types.push("all-but-norm"); unc_pens.push(green); unc_labels.push("all except normalisation");

xSizeDef = 8cm;
ySizeDef = 6cm;

A_ref = 606.;
B_ref = 20.4;
ref_str = MakeRefStr();


//----------------------------------------------------------------------------------------------------

void DrawUncRelBand(RootObject bc, RootObject relUnc, pen p)
{
	int N = bc.iExec("GetN");

	guide g;
	guide g_u, g_b;

	for (int i = 0; i < N; ++i)
	{
		real ta[] = {0.};
		real sa[] = {0.};

		bc.vExec("GetPoint", i, ta, sa);
		real ru = relUnc.rExec("Eval", ta);

		real y_ref = A_ref * exp(- B_ref * ta[0]);

		real c_rel = sa[0] / y_ref - 1.;
		real c_rel_pl = sa[0] * (1. + ru) / y_ref - 1.;
		real c_rel_mi = sa[0] * (1. - ru) / y_ref - 1.;

		g_u = g_u -- Scale((ta[0], c_rel_pl));
		g_b = g_b -- Scale((ta[0], c_rel_mi));
	}

	g_b = reverse(g_b);
	filldraw(g_u--g_b--cycle, p, nullpen);
}

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "${\d\si/\d t - \hbox{ref} \over \hbox{ref}}\ ,\ \ \hbox{ref} = $ "+ref_str+"");
currentpad.xTicks = LeftTicks(0.05, 0.01);
currentpad.yTicks = RightTicks(0.02, 0.01);

AddToLegend("<systematic uncertainties:");
for (int ui : unc_types.keys)
{
	RootObject fit = RootGetObject(fit_file, fit_obj);
	RootObject relUnc = RootGetObject(unc_file, "matrices/" + unc_types[ui] + "/" + diagonal + "/g_envelope");
	DrawUncRelBand(fit, relUnc, unc_pens[ui]);
	AddToLegend(unc_labels[ui], mSq+6pt+unc_pens[ui]);
}

AddToLegend("<data with statistical uncertainties:");
DrawRelDiff(
	RootGetObject(topDir+"DS-merged/merged.root", binning+"/"+dataset+"/"+diagonal+"/h_dsdt"),
	red+0.8pt);
AddToLegend("data", mPl+4pt+(red+0.8pt));

limits((0, -0.03), (0.25, 0.07), Crop);
xaxis(YEquals(0, false), dashed);

AttachLegend();
