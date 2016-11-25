import root;
import pad_layout;

string topDir = "../../";

string datasets[];
datasets.push("DS-fill5313");
datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
datasets.push("DS-fill5321");

string diagonals[] = { "45b_56t", "45t_56b" };
//string diagonals[] = { "45b_56t" };

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("L-220-fr");
rps.push("L_1_F"); rp_labels.push("L-210-fr");
rps.push("R_1_F"); rp_labels.push("R-210-fr");
rps.push("R_2_F"); rp_labels.push("R-220-fr" );


xSizeDef = 6cm;
ySizeDef = 5cm;
//yTicksDef = RightTicks(5., 1.);
//xTicks=LeftTicks(format="$$", 20., 10.)
xTicksDef = LeftTicks(50., 10.);

int gx=0, gy=0;

TH2_palette = Gradient(blue, heavygreen, yellow, red);
TH2_z_min = 0;
TH2_z_max = 1.;

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
		NewPad(false, -1, gy);	
		label("{\SetFontSizesXX efficiency}");

		for (int rpi : rps.keys)
		{
			string d = diagonals[dgi] + "/" + rps[rpi];

			TH2_z_min = 0.9; TH2_z_max = 1.0;
			TH2_paletteTicks = PaletteTicks(Step=0.02, step=0.01);

			++gx;
			NewPad("$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", gx, gy, axesAbove=true);
			draw(scale(1., sgn), RootGetObject(f, d+"/track/th_x, th_y : rel"), "def");

			limits((-150, 0), (150, 120), Crop);
		}
		
		++gy; gx = 0;
		NewPad(false, -1, gy);	
		label("{\SetFontSizesXX more than 1 track}");

		for (int rpi : rps.keys)
		{
			string d = diagonals[dgi] + "/" + rps[rpi];

			TH2_z_min = 0.; TH2_z_max = 0.1;
			
			++gx;
			NewPad("$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", gx, gy);
			draw(scale(1., sgn), RootGetObject(f, d+"/pl_suff_no_track/th_x, th_y : rel"), "def");

			limits((-150, 0), (150, 120), Crop);
		}
	}
}

GShipout(vSkip=0pt);
