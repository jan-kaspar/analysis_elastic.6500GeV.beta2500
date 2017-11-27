import root;
import pad_layout;

string topDir = "../../";

string datasets[] = {
	"DS-fill5313"
};

string diagonals[] = { "45b_56t", "45t_56b" };
string diagonal_labels[] = { "45b -- 56t", "45t -- 56b" };

string model = "fit3-2";

string binning = "ob-3-5-0.05";

//xSizeDef = 5.1cm;
//ySizeDef = 5.1cm;

real y_min = 0.995, y_max = 1.005;

//----------------------------------------------------------------------------------------------------

for (string dataset : datasets)
{
	NewPad();

	for (int dgni : diagonals.keys)
	{
		NewRow();

		NewPad(false);
		label("\vbox{\SetFontSizesXX\hbox{" + dataset + "}\hbox{" + diagonal_labels[dgni] + "}}");

		string f_ni = topDir + dataset + "/unfolding_cf_ni_" + diagonals[dgni] + ".root";
		string f_mc = topDir + dataset + "/unfolding_cf_mc_" + diagonals[dgni] + ".root";


		// ----- full |t| range -----

		NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction", yTicks=RightTicks(0.005, 0.001));

		draw(RootGetObject(f_mc, model + "/" + binning + "/h_corr"), "eb", blue);

		draw(RootGetObject(f_ni, model + "/g_t_corr"), red);

		limits((0, y_min), (1., y_max), Crop);


		// ----- low |t| zoom -----

		NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction", yTicks=RightTicks(0.005, 0.001));

		draw(RootGetObject(f_mc, model + "/" + binning + "/h_corr"), "eb", blue, "Monte Carlo");

		draw(RootGetObject(f_ni, model + "/g_t_corr"), red, "numerical integration");

		limits((0, y_min), (0.01, y_max), Crop);

		yaxis(XEquals(8e-4, false), dashed);

		AttachLegend(dataset + ", " + diagonal_labels[dgni], NW, NE);
	}
}

GShipout(margin=1mm);
