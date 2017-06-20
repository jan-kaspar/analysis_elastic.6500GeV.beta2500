import root;
import pad_layout;

string topDir = "../";

string datasets[];
datasets.push("DS-fill5313");
datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
datasets.push("DS-fill5321");

string diagonals[], dgn_labels[];
diagonals.push("45b_56t"); dgn_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); dgn_labels.push("45 top -- 56 bot");

string selection = "matching input, full";
//string selection = "matching input, vtx_x_0.3sigma";
//string selection = "matching input, vtx_x_0.1sigma";

xSizeDef = 10cm;
ySizeDef = 10cm;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	for (int dgni : diagonals.keys)
	{
		NewPad("$(y^{\rm L-220-F} - y^{\rm R-220-F}) / \sqrt 2\ung{mm}$", "$(y^{\rm L-220-F} + y^{\rm R-220-F}) / \sqrt 2\ung{mm}$");

		AddToLegend("<{\it"+dgn_labels[dgni]+":}");

		string f = topDir + datasets[dsi] + "/distributions_" + diagonals[dgni] + ".root";
		draw(RootGetObject(f, "optics/"+selection+"/p_y_220_F_rot"), "eb", black);

		RootObject fit = RootGetObject(f, "optics/"+selection+"/p_y_220_F_rot|pol1");
		draw(fit, "l", red+1pt);

		real slope = fit.rExec("GetParameter", 1);
		real slope_unc = fit.rExec("GetParError", 1);
		real a = -2 * slope;
		real a_unc = 2 * slope_unc;
		AddToLegend(format("$a = %#+.4f$", a) + format("$\pm %#.4f$", a_unc), black);
	
		AttachLegend();
	}
}

