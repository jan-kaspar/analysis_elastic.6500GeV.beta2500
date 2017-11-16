import root;
import pad_layout;

string topDir = "../../";

string datasets[] = {
	"DS-fill5313",
	"DS-fill5314",
	"DS-fill5317",
	"DS-fill5321",
};

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

string binning = "ob-2-10-0.05";

TH1_x_min = 8e-4;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();
	
	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}}");

	for (int dgni : diagonals.keys)
	{
		string f = topDir + datasets[dsi] + "/distributions_" + diagonals[dgni] + ".root";

		NewPad("$|t|\ung{GeV^2}$", "full acceptance correction");
		scale(Log, Linear);

		draw(RootGetObject(f, "acceptance correction/" + binning + "/p_t_full_corr"), "eb", red);

		limits((8e-4, 1), (1, 15), Crop);

		AttachLegend(diagonal_labels[dgni]);
	}
}

GShipout(margin=1mm, hSkip=1mm);
