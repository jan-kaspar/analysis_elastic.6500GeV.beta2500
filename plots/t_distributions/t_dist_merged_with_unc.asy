import root;
import pad_layout;

string topDir = "../../";

//string binning = "ob-1-20-0.05";
string binning = "ob-2-10-0.05";

string diagonal = "combined";

string fit_file = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/first_fits/2/do_fit.root";
string fit_obj = "variant 2/g_dsdt_CH";

string unc_file = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/2rp/systematics/DS-fill5313/matrix_numerical_integration.root";
string unc_types[], unc_labels[];
pen unc_pens[];
unc_types.push("all"); unc_pens.push(blue+opacity(0.5)); unc_labels.push("all");
unc_types.push("all-but-norm"); unc_pens.push(green); unc_labels.push("all except normalisation");

drawGridDef = true;

TH1_x_min = 8.1e-4;

//----------------------------------------------------------------------------------------------------

void DrawUncBand(RootObject bc, RootObject relUnc, pen p)
{
	int N = bc.iExec("GetN");

	guide g_u, g_b;
	for (int i = 0; i < N; ++i)
	{
		real ta[] = {0.};
		real sa[] = {0.};

		bc.vExec("GetPoint", i, ta, sa);
		real ru = relUnc.rExec("Eval", ta);

		g_u = g_u -- Scale((ta[0], sa[0] * (1. + ru)));
		g_b = g_b -- Scale((ta[0], sa[0] * (1. - ru)));
	}

	g_b = reverse(g_b);
	filldraw(g_u--g_b--cycle, p, nullpen);
}

//----------------------------------------------------------------------------------------------------

void PlotEverything()
{
	AddToLegend("<systematic uncertainties:");
	for (int ui : unc_types.keys)
	{
		RootObject fit = RootGetObject(fit_file, fit_obj);
		RootObject relUnc = RootGetObject(unc_file, "matrices/" + unc_types[ui] + "/" + diagonal + "/g_envelope");
		DrawUncBand(fit, relUnc, unc_pens[ui]);
		AddToLegend(unc_labels[ui], mSq+6pt+unc_pens[ui]);
	}

	AddToLegend("<data with statistical uncertainties:");
	draw(RootGetObject(topDir+"DS-merged/merged.root", binning + "/merged/" + diagonal + "/h_dsdt"), "d0,eb", red+0.8pt);
	AddToLegend("data", mPl+4pt+(red+0.8pt));
}

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$", 8cm, 6cm);
scale(Linear, Log);

PlotEverything();

currentpad.xTicks = LeftTicks(0.2, 0.1);
limits((0, 1e-3), (1, 1e3), Crop);

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$", 8cm, 6cm);

PlotEverything();

currentpad.xTicks = LeftTicks(0.002, 0.001);
limits((0, 400), (0.01, 1000), Crop);

AttachLegend();
