import root;
import pad_layout;

string topDir = "../../";

string datasets[], dataset_fills[];
int dataset_periods[][];
datasets.push("DS-fill5313"); dataset_fills.push("5313"); dataset_periods.push(new int[] {0, 1, 2});
datasets.push("DS-fill5314"); dataset_fills.push("5314"); dataset_periods.push(new int[] {5, 6, 7});
datasets.push("DS-fill5317"); dataset_fills.push("5317"); dataset_periods.push(new int[] {10, 14, 18});
datasets.push("DS-fill5321"); dataset_fills.push("5321"); dataset_periods.push(new int[] {24, 26, 28});

string diagonals[], dgn_labels[];
pen dgn_pens[];
//diagonals.push("45b_56t"); dgn_labels.push("45b -- 56t"); dgn_pens.push(red);
diagonals.push("45t_56b"); dgn_labels.push("45t -- 56b"); dgn_pens.push(blue);

string quantities[], q_file_tags[], q_labels[];
real q_fit_maxs[], q_ranges[];
quantities.push("th_x"); q_file_tags.push("_dx"); q_labels.push("\th^{*R}_x - \th^{*L}_x"); q_fit_maxs.push(50); q_ranges.push(60);
quantities.push("th_y"); q_file_tags.push("_dy"); q_labels.push("\th^{*R}_y - \th^{*L}_y"); q_fit_maxs.push(1.5); q_ranges.push(2);

//----------------------------------------------------------------------------------------------------

for (int qi : quantities.keys)
{
	for (int dsi : datasets.keys)
	{
		NewRow();

		NewPad(false);
		label("{\SetFontSizesXX " + dataset_fills[dsi] + "}");

		for (int period : dataset_periods[dsi])
		{
			NewPad("$" + q_labels[qi] + "\ung{\mu rad}$");
			scale(Linear, Log);

			for (int dgni : diagonals.keys)
			{
				string f = topDir + datasets[dsi] + "/distributions_" + diagonals[dgni] + ".root";
				string bP = "selected - angles/period" + format("%u", period) + "/h_" + quantities[qi] + "_LRdiff";

				draw(scale(1e6, 1), RootGetObject(f, bP), "eb", dgn_pens[dgni]);

				TF1_x_min = -q_fit_maxs[qi]*1e-6;
				TF1_x_max = +q_fit_maxs[qi]*1e-6;
				draw(scale(1e6, 1), RootGetObject(f, bP+"|gaus"), black);
			}

			xlimits(-q_ranges[qi], +q_ranges[qi], Crop);

			AttachLegend("period" + format("%u", period));
		}

	}

	GShipout("non_gaussianity" + q_file_tags[qi]);
}
