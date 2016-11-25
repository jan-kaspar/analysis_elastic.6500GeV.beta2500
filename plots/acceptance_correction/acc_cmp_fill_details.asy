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
//diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgni : diagonals.keys)
	{
		NewRow();

		NewPad(false);
		label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}\hbox{" + diagonal_labels[dgni]+ "}}");

		string f = top_dir+"/"+datasets[dsi]+"/distributions_" + diagonals[dgni] + ".root";

		if (diagonals[dgni] == "45b_56t")
		{
			TH2_y_min = 0e-6;
			TH2_y_max = +15e-6;
		} else {
			TH2_y_min = -15e-6;
			TH2_y_max = 0e-6;
		}

		yTicksDef = RightTicks(5., 1.);

		// ----- left arm -----

		NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", axesAbove=true);
		currentpad.xTicks = LeftTicks(50., 10.);
		scale(Linear, Linear, Log);
		draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/h_th_y_L_vs_th_x_L"), "def");

		if (diagonals[dgni] == "45b_56t")
			draw((-150, 4)--(-25, 4)--(95, 10), black+2pt);
		else
			draw((-100, -10)--(+15, -4)--(150, -4), black+2pt);
		

		limits((-150, TH2_y_min*1e6), (150, TH2_y_max*1e6), Crop);
		AttachLegend("left arm");

		// ----- right arm -----

		NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", axesAbove=true);
		currentpad.xTicks = LeftTicks(50., 10.);
		scale(Linear, Linear, Log);
		draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/h_th_y_R_vs_th_x_R"), "def");

		if (diagonals[dgni] == "45b_56t")
			draw((-150, 4.5)--(-10, 4.5)--(105, 10), black+2pt);
		else
			draw((-110, -10)--(+10, -4)--(150, -4), black+2pt);


		limits((-150, TH2_y_min*1e6), (150, TH2_y_max*1e6), Crop);
		AttachLegend("right arm");
	}
}

GShipout(margin=1mm, hSkip=1mm);
