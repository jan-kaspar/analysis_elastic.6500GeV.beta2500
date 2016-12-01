import root;
import pad_layout;

string topDir = "../../";

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5321"); dataset_fills.push("5321");

string diagonals[], diagonal_labels[];
pen diagonal_pens[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top"); diagonal_pens.push(blue);
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot"); diagonal_pens.push(red);

string arms[], arm_ss[], arm_labels[];
arms.push("_L"); arm_ss.push("L"); arm_labels.push("left arm");
arms.push("_R"); arm_ss.push("R"); arm_labels.push("right arm");
arms.push(""); arm_ss.push(""); arm_labels.push("double arm");

xSizeDef = 12cm;
//xTicksDef = LeftTicks(Step=1, step=0.5);

//TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int ai : arms.keys)
{
	NewPad(false);
	label("{\SetFontSizesXX " + arm_labels[ai] + " arm}");
}

//----------------------------------------------------------------------------------------------------

frame f_legend;

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	for (int ai : arms.keys)
	{
		NewPad("$\th_y^{*"+arm_ss[ai]+"}\ung{\mu rad}$", "mean of~$\th_x^{*"+arm_ss[ai]+"}$");
		for (int dgni : diagonals.keys)
		{
			TF1_x_min = -inf;
			TF1_x_max = +inf;

			string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";
			
			draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/p_th_x"+arms[ai]+"_vs_th_y"+arms[ai]),
				"eb,d0", StdPen(dgni+1), diagonal_labels[dgni]);
			RootObject fit = RootGetObject(f, "selected - angles/p_th_x"+arms[ai]+"_vs_th_y"+arms[ai]+"|pol1");
			draw(scale(1e6, 1e6), fit, black+dashed, (dgni == 0) ? "single-RP fit" : "");
	
			real a = fit.rExec("GetParameter", 1);
			real a_u = fit.rExec("GetParError", 1);
	
			real x = (dgni == 0) ? +60 : -60;
			label(format("slope = $%.3f$", a) + format("$\pm %.3f$", a_u), (x, -2));
		}

		limits((-90, -3), (90, 3), Crop);
	}
	
	f_legend = BuildLegend();

	NewPad(false);
	add(f_legend);
}

GShipout(vSkip=0mm);
