import root;
import pad_layout;

string topDir = "../../";

string datasets[] = {
	"DS-fill5313"
};

string diagonals[] = { "45b_56t", "45t_56b" };
string diagonal_labels[] = { "45b -- 56t", "45t -- 56b" };

//xSizeDef = 5.1cm;
//ySizeDef = 5.1cm;

//----------------------------------------------------------------------------------------------------

for (string dataset : datasets)
{
	NewPad();

	for (int dgni : diagonals.keys)
	{
		string f_ni = topDir + dataset + "/unfolding_cf_" + diagonals[dgni] + ".root";
		//string f_mc = topDir + "unsmearing_correction/analyzeCorrections_predefined:none.root";
	
		NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction", yTicks=RightTicks(0.01, 0.002));
		draw(RootGetObject(f_ni, "first_fit/g_t_corr"), red);
	
		/*
		for (int bi : binnings.keys)
		{
			draw(rGetObj(f_mc, "fit_models_merged/h_mean"), "eb", StdPen(bi + 1));
		}
		*/
	
		limits((0, 0.97), (0.01, 1.01), Crop);

		yaxis(XEquals(8e-4, false), dashed);
	
		AttachLegend(dataset + ", " + diagonal_labels[dgni]);
	}
}


GShipout(margin=1mm);
