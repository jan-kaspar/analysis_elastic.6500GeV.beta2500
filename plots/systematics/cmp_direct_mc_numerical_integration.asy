import root;
import pad_layout;
import common_code;

string topDir = "../../";

xSizeDef = 18cm;

string datasets[] = { "DS-fill5313" };

//string diagonals[] = { "45b_56t", "45t_56b" };
string diagonals[] = { "45b_56t" };
//string diagonals[] = { "45t_56b" };

string t_dist_type = "first fit";

AddAllModes();

//----------------------------------------------------------------------------------------------------

bool legend_drawn = false;
int idx = 0;

for (int dsi : datasets.keys)
{
	for (int dgni : diagonals.keys)
	{
		for (int xsi = 0; xsi < 2; ++xsi)
		{	
			++idx;
	
			NewPad(false, idx, -1);
			string title = (xsi == 0) ? "low-$|t|$ zoom" : "full $|t|$ range";
			label("{\SetFontSizesXX "+replace("\vbox{\hbox{"+datasets[dsi]+"}"
				"\hbox{dgn: "+diagonals[dgni]+"}\hbox{"+title+"}}", "_", "\_")+"}");
	
			for (int mi : modes.keys)
			{
				//write(modes[mi].tag);
	
				if (idx == 1)
				{
					NewPad(false, 0, mi);
					label("{\SetFontSizesXX " + modes[mi].label + "}");
				}
				
				NewPad("$|t|\ung{GeV^2}$", "systematic effect", idx, mi);
				currentpad.xSize = (xsi == 0) ? 6cm : 12cm;

				real x_min = 0, x_max = 0.3, x_Step = 0.05, x_step = 0.01;
				real y_min = -0.02, y_max = +0.02, y_Step = 0.005, y_step = 0.001;
	
				if (xsi == 0)
					{ x_min = 0; x_max = 0.01; x_Step = 0.002; x_step = 0.001; }

				if (modes[mi].mc_dir_tag == "norm")
					{ y_min = 0; y_max = +0.15; y_Step = 0.05; y_step = 0.01; }
	
				// ----- MC direct -----
	
				/*
				string mc_f = (modes[mi].mc_dir_file == "sd")
					? topDir + "systematics/"+datasets[dsi]+"/simu_direct_"+diagonals[dgni]+".root"
					: topDir + "systematics/"+datasets[dsi]+"/simu_direct_unsmearing_"+diagonals[dgni]+".root";
			
				string objPath = modes[mi].mc_dir_tag + ((modes[mi].mc_dir_file == "sd") ? "/h_eff_syst" : "");
				objPath = replace(objPath, "<TDIST>", t_dist_type);

				RootObject os = RootGetObject(mc_f, objPath, error=false);
				if (os.valid)
					draw(shift(0, -modes[mi].mc_dir_ref), os, "eb", red, "Monte-Carlo (direct)");
				*/
				
				// ----- numerical integration, simple -----
		
				string ni_f = topDir + "systematics/"+datasets[dsi]+"/numerical_integration_"+diagonals[dgni]+".root";
	
				string objPath = (modes[mi].num_int_file == "s")
					?  "<TDIST>/" + modes[mi].num_int_tag + "/g_eff"
					: modes[mi].num_int_tag;
	
				objPath = replace(objPath, "<TDIST>", t_dist_type);
	
				RootObject os = RootGetObject(ni_f, objPath, error=false);
				if (os.valid)
					draw(shift(0, -modes[mi].num_int_ref), os, "l,d0", blue+1pt, "numerical intergration");
	

				// ----- numerical integration, full -----
		
				string ni_f = topDir + "systematics/"+datasets[dsi]+"/numerical_integration_full_"+diagonals[dgni]+".root";
	
				string objPath = "<TDIST>/" + modes[mi].num_int_tag + "/g_eff";
				objPath = replace(objPath, "<TDIST>", t_dist_type);
	
				RootObject os = RootGetObject(ni_f, objPath, error=false);
				if (os.valid)
					draw(shift(0, -modes[mi].num_int_ref), os, "l,d0", red+dashed+1pt, "numerical intergration, full");
	

				// ----- finalisation -----

				currentpad.xTicks = LeftTicks(x_Step, x_step);
				currentpad.yTicks = RightTicks(y_Step, y_step);
			
				limits((0, y_min), (x_max, y_max), Crop);
			
				xaxis(YEquals(0, false), dashed);
		
				if (!legend_drawn)
				{
					frame f_legend = BuildLegend();
					NewPad(false, 0, -1);
					add(f_legend);
					legend_drawn = true;
				}
			}
		}
	}
}
