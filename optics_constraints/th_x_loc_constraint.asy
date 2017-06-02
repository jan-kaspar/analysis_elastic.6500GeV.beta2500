import root;
import pad_layout;

string topDir = "../";

string datasets[];
datasets.push("DS-fill5313");
datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
datasets.push("DS-fill5321");

string diagonals[], dgn_labels[];
pen dgn_pens_data[], dgn_pens_fit[];
diagonals.push("45b_56t"); dgn_labels.push("45 bot -- 56 top"); dgn_pens_data.push(black); dgn_pens_fit.push(green);
diagonals.push("45t_56b"); dgn_labels.push("45 top -- 56 bot"); dgn_pens_data.push(blue); dgn_pens_fit.push(red);

//string selection = "matching input, full";
//string selection = "matching input, vtx_x_0.3sigma";
string selection = "matching input, vtx_x_0.1sigma";

xSizeDef = 10cm;
ySizeDef = 10cm;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	//--------------------

	NewPad("$\th_x^{\rm loc,L}\ung{\mu rad}$", "$\th_x^{\rm loc,R}\ung{\mu rad}$");
	for (int dgni : diagonals.keys)
	{
		AddToLegend("<{\it"+dgn_labels[dgni]+":}");

		string f = topDir + datasets[dsi] + "/distributions_" + diagonals[dgni] + ".root";
		draw(scale(1e6, 1e6), RootGetObject(f, "optics/"+selection+"/p_thl_x_L_vs_thl_x_R"), "eb", dgn_pens_data[dgni]);

		RootObject fit = RootGetObject(f, "optics/"+selection+"/p_thl_x_L_vs_thl_x_R|pol1");
		draw(scale(1e6, 1e6), fit, "l", dgn_pens_fit[dgni]);

		real slope = fit.rExec("GetParameter", 1);
		real slope_unc = fit.rExec("GetParError", 1);
		real a = slope + 1.;
		real a_unc = slope_unc;
		AddToLegend(format("$a = %#+.4f$", a) + format("$\pm %#.4f$", a_unc), dgn_pens_fit[dgni]);
	}
	AttachLegend();

	//--------------------

	NewPad("$\th_x^{\rm loc,R}\ung{\mu rad}$", "$\th_x^{\rm loc,L}\ung{\mu rad}$");
	for (int dgni : diagonals.keys)
	{
		AddToLegend("<{\it"+dgn_labels[dgni]+":}");

		string f = topDir + datasets[dsi] + "/distributions_" + diagonals[dgni] + ".root";
		draw(scale(1e6, 1e6), RootGetObject(f, "optics/"+selection+"/p_thl_x_R_vs_thl_x_L"), "eb", dgn_pens_data[dgni]);

		RootObject fit = RootGetObject(f, "optics/"+selection+"/p_thl_x_R_vs_thl_x_L|pol1");
		draw(scale(1e6, 1e6), fit, "l", dgn_pens_fit[dgni]);

		real slope = fit.rExec("GetParameter", 1);
		real slope_unc = fit.rExec("GetParError", 1);
		real a = -slope - 1.;
		real a_unc = slope_unc;
		AddToLegend(format("$a = %#+.4f$", a) + format("$\pm %#.4f$", a_unc), dgn_pens_fit[dgni]);
	}
	AttachLegend();

	//--------------------

	NewPad("$(\th_x^{\rm loc,L} - \th_x^{\rm loc,R}) / \sqrt 2\ung{\mu rad}$", "$(\th_x^{\rm loc,L} + \th_x^{\rm loc,R}) / \sqrt 2\ung{\mu rad}$");
	for (int dgni : diagonals.keys)
	{
		AddToLegend("<{\it"+dgn_labels[dgni]+":}");

		string f = topDir + datasets[dsi] + "/distributions_" + diagonals[dgni] + ".root";
		draw(scale(1e6, 1e6), RootGetObject(f, "optics/"+selection+"/p_thl_x_rot"), "eb", dgn_pens_data[dgni]);

		RootObject fit = RootGetObject(f, "optics/"+selection+"/p_thl_x_rot|pol1");
		draw(scale(1e6, 1e6), fit, "l", dgn_pens_fit[dgni]+1pt);

		real slope = fit.rExec("GetParameter", 1);
		real slope_unc = fit.rExec("GetParError", 1);
		real a = -2 * slope;
		real a_unc = 2 * slope_unc;
		AddToLegend(format("$a = %#+.4f$", a) + format("$\pm %#.4f$", a_unc), dgn_pens_fit[dgni]);
	}
	AttachLegend();
}
