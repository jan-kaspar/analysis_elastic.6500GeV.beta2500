import root;
import pad_layout;

string topDir = "../../";

string f = topDir + "background_studies/correction.root";

string hists[], h_labels[];
pen h_pens[];
hists.push("h_bb_bt"); h_labels.push("anti-diagonal 45 bot -- 56 bot / diagonal 45 bot -- 56 top"); h_pens.push(red);
hists.push("h_tt_bt"); h_labels.push("anti-diagonal 45 top -- 56 top / diagonal 45 bot -- 56 top"); h_pens.push(blue);
hists.push("h_bb_tb"); h_labels.push("anti-diagonal 45 bot -- 56 bot / diagonal 45 top -- 56 bot"); h_pens.push(heavygreen);
hists.push("h_tt_tb"); h_labels.push("anti-diagonal 45 top -- 56 top / diagonal 45 top -- 56 bot"); h_pens.push(cyan);

//----------------------------------------------------------------------------------------------------

void DrawAll()
{
	for (int hi : hists.keys)
		draw(RootGetObject(f, "cmp|" + hists[hi]), "eb,d0", h_pens[hi], h_labels[hi]);

	draw(RootGetObject(f, "g_corr"), "l", black, "fit");
}

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "B / (S+B)");
DrawAll();
limits((0, 0), (1., 0.15), Crop);

NewPad("$|t|\ung{GeV^2}$", "B / (S+B)");
DrawAll();
limits((0, 0), (0.2, 0.005), Crop);

AttachLegend(NW, NE);
