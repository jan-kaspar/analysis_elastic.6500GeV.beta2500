import root;
import pad_layout;

string topDir = "../../";

string datasets[];
datasets.push("DS-fill5313");
/*
datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
datasets.push("DS-fill5321");
*/

string diagonals[] = { "45b_56t", "45t_56b" };

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("L-220-fr");
rps.push("L_1_F"); rp_labels.push("L-210-fr");
rps.push("R_1_F"); rp_labels.push("R-210-fr");
rps.push("R_2_F"); rp_labels.push("R-220-fr" );


xSizeDef = 6cm;
ySizeDef = 5cm;
yTicksDef = RightTicks(5., 1.);

int gx=0, gy=0;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgi : diagonals.keys)
	{
		string f = topDir + datasets[dsi] + "/eff3outof4_details.root";
		real sgn = (diagonals[dgi] == "45b_56t") ? +1 : -1;
		string opt = "vl,eb";
		
		++gy; gx = 0;
		for (int rpi : rps.keys)
		{
			++gx;
			NewPad(false, gx, gy);
			label("{\SetFontSizesXX " + rp_labels[rpi] + "}");
		}
		
		NewPad(false, -1, gy);
		label(replace("\vbox{\SetFontSizesXX\hbox{dataset: "+datasets[dsi]+"}\hbox{diagonal: "+diagonals[dgi]+"}}", "_", "\_"));

		++gy; gx = 0;
		for (int rpi : rps.keys)
		{
			string d = diagonals[dgi] + "/" + rps[rpi];

			++gx;
			NewPad("", "\ung{\%}", gx, gy);
			//currentpad.xTicks=LeftTicks(format="$$", 20., 10.);
			draw(scale(sgn, 100), RootGetObject(f, d+"/anything/th_y : rel"), opt, black, "anything");
			draw(scale(sgn, 100), RootGetObject(f, d+"/track/th_y : rel"), opt, cyan, "track");
			draw(scale(sgn, 100), RootGetObject(f, d+"/track_compatible/th_y : rel"), opt, magenta, "compatible track");

			limits((0, 80), (100, 100), Crop);
		}

		frame f_legend = BuildLegend();
		NewPad(false, ++gx, gy);
		add(f_legend);
		
		++gy; gx = 0;
		for (int rpi : rps.keys)
		{
			string d = diagonals[dgi] + "/" + rps[rpi];
			
			++gx;
			NewPad("$\th_y^*\ung{\mu rad}$", "\ung{\%}", gx, gy);
			draw(scale(sgn, 100), RootGetObject(f, d+"/pl_insuff/th_y : rel", error=false), opt, red, "pl insuff");
			draw(scale(sgn, 100), RootGetObject(f, d+"/pl_suff_no_track/th_y : rel"), opt, blue, "pl suff no track");
			draw(scale(sgn, 100), RootGetObject(f, d+"/pat_more/th_y : rel"), opt, heavygreen, "pat more");

			limits((0, 0), (100, 20), Crop);
		}

		frame f_legend = BuildLegend();
		NewPad(false, ++gx, gy);
		add(f_legend);
	}
}

GShipout(vSkip=0pt);
