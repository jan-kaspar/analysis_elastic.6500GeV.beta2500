import root;
import pad_layout;

string top_dir = "../../";

string datasets[] = {
	"DS-fill5313",
	"DS-fill5314",
	"DS-fill5317",
	"DS-fill5321",
};

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

string cuts[], cut_labels[];
cuts.push("L"); cut_labels.push("left arm");
cuts.push("R"); cut_labels.push("right arm");
cuts.push("G"); cut_labels.push("global");

xSizeDef = 8cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();
	
	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}}");

	for (int dgni : diagonals.keys)
	{
		string f = top_dir+"/"+datasets[dsi]+"/distributions_" + diagonals[dgni] + ".root";

		TH2_x_min = -50e-6;
		TH2_x_max = +50e-6;

		if (diagonals[dgni] == "45b_56t")
		{
			TH2_y_min = 3e-6;
			TH2_y_max = +9e-6;
		} else {
			TH2_y_min = -9e-6;
			TH2_y_max = -3e-6;
		}

		yTicksDef = RightTicks(1., 0.5);

		NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$");
		currentpad.xTicks = LeftTicks(50., 10.);

		for (int ci : cuts.keys)
		{
			RootObject obj = RootGetObject(f, "fiducial cuts/fc_"+cuts[ci]+"_l");
			draw(scale(1e6, 1e6), obj, "l", StdPen(ci+1), cut_labels[ci]);

			if (cuts[ci] != "G")
			{
				real sh = 2;
			
				//draw(shift(+sh, 0)*scale(1e6, 1e6), obj, "l", StdPen(ci+1) + dashed);
				//draw(shift(-sh, 0)*scale(1e6, 1e6), obj, "l", StdPen(ci+1) + dashed);
			}

			if (cuts[ci] == "G")
			{
				real y = obj.rExec("Eval", 0) * 1e6;

				draw((0, y), mCi+3pt+StdPen(ci+1));
				pair alig = (diagonals[dgni] == "45b_56t") ? NW : SE;
				label(format("%#.3f", y), (0, y), alig, StdPen(ci+1));
			}
		}

		limits((TH2_x_min*1e6, TH2_y_min*1e6), (TH2_x_max*1e6, TH2_y_max*1e6), Crop);
		AttachLegend(diagonal_labels[dgni], NW, NW);
	}
}

GShipout(margin=1mm, hSkip=1mm);
