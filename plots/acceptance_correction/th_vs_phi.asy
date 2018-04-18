import root;
import pad_layout;

string top_dir = "../../";

string dataSets[] = {
	"DS-fill5317"
};

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

for (int dsi : dataSets.keys)
{
	real ySize = 6cm;

	NewPad("$\ph^{*}\ung{rad}$", "$\th^{*}\ung{\mu rad}$", ySize*2, ySize);
	scale(Linear, Linear, Log);
	//TH2_zLabel = "(corrected) events per bin";
	TH2_paletteBarWidth = 0.05;

	// z scale
	//TH2_z_min = 5.5;
	//TH2_z_max = 3.75;

	// 45 bottom - 56 top
	string f = top_dir+"/"+dataSets[dsi]+"/distributions_45b_56t.root";
	draw(scale(1e0, 1e6), RootGetObject(f, "acceptance correction/h_th_vs_phi_after"), "def");

	// 45 top - 56 bottom
	string f = top_dir+"/"+dataSets[dsi]+"/distributions_45t_56b.root";
	draw(scale(1e0, 1e6), RootGetObject(f, "acceptance correction/h_th_vs_phi_after"), "def");

	limits((-3.141593, 0), (+3.141593, 200), Crop);
	AttachLegend(dataSets[dsi]);
}

GShipout(margin=0mm);
