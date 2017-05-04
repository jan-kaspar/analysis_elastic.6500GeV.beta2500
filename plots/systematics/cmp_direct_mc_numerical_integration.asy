import root;
import pad_layout;
import common_code;

string topDir = "../../";

xSizeDef = 6cm;

string datasets[] = { "DS-fill5313" };

//string diagonals[] = { "45b_56t", "45t_56b" };
string diagonals[] = { "45b_56t" };
//string diagonals[] = { "45t_56b" };

real z_t_maxs[], z_t_Steps[], z_t_steps[], z_e_maxs[], z_e_Steps[], z_e_steps[];
z_t_maxs.push(0.004); z_t_Steps.push(0.002); z_t_steps.push(0.001); z_e_maxs.push(0.02); z_e_Steps.push(0.005); z_e_steps.push(0.001);
z_t_maxs.push(0.2); z_t_Steps.push(0.05); z_t_steps.push(0.01); z_e_maxs.push(0.02); z_e_Steps.push(0.005); z_e_steps.push(0.001);
z_t_maxs.push(1.0); z_t_Steps.push(0.2); z_t_steps.push(0.1); z_e_maxs.push(0.04); z_e_Steps.push(0.01); z_e_steps.push(0.005);

string t_dist_type = "fit2-2";

AddAllModes();

//----------------------------------------------------------------------------------------------------

bool legend_drawn = false;
int idx = 0;

for (int dsi : datasets.keys)
{
	for (int dgni : diagonals.keys)
	{
		for (int zi : z_t_maxs.keys)
		{	
			++idx;
	
			NewPad(false, idx, -1);
			label("{\SetFontSizesXX "+replace("\vbox{\hbox{"+datasets[dsi]+"}"
				"\hbox{dgn: "+diagonals[dgni]+"}}", "_", "\_")+"}");
	
			for (int mi : modes.keys)
			{
				//write(modes[mi].tag);
	
				if (idx == 1)
				{
					NewPad(false, 0, mi);
					label("{\SetFontSizesXX " + modes[mi].label + "}");
				}
				
				NewPad("$|t|\ung{GeV^2}$", "systematic effect", idx, mi);
	
				// ----- MC direct -----
	
				string mc_f = topDir + "systematics-mc/"+datasets[dsi]+"/"+diagonals[dgni]+"/simu_direct_" + modes[mi].num_int_tag + "/1E8.root";
			
				string objPath = "re/h_eff_syst";

				RootObject os = RootGetObject(mc_f, objPath, error=false);
				if (os.valid)
					draw(shift(0, -modes[mi].mc_dir_ref), os, "eb", heavygreen, "Monte-Carlo (direct)");
	

				// ----- numerical integration, full -----
		
				string ni_f = topDir + "systematics/"+datasets[dsi]+"/numerical_integration_full_"+diagonals[dgni]+".root";
	
				string objPath = "<TDIST>/" + modes[mi].num_int_tag + "/g_eff";
				objPath = replace(objPath, "<TDIST>", t_dist_type);
	
				RootObject os = RootGetObject(ni_f, objPath, error=false);
				if (os.valid)
					draw(shift(0, -modes[mi].num_int_ref), os, "l,d0", red+1pt, "numerical intergration, full");
				
				// ----- numerical integration, simple -----
		
				string ni_f = topDir + "systematics/"+datasets[dsi]+"/numerical_integration_"+diagonals[dgni]+".root";
	
				string objPath = (modes[mi].num_int_file == "s")
					?  "<TDIST>/" + modes[mi].num_int_tag + "/g_eff"
					: modes[mi].num_int_tag;
	
				objPath = replace(objPath, "<TDIST>", t_dist_type);
	
				RootObject os = RootGetObject(ni_f, objPath, error=false);
				if (os.valid)
					draw(shift(0, -modes[mi].num_int_ref), os, "l,d0", blue+dashed+1pt, "numerical intergration");
	

				// ----- finalisation -----

				real t_Step = z_t_Steps[zi];
				real t_step = z_t_steps[zi];
				real e_Step = z_e_Steps[zi];
				real e_step = z_e_steps[zi];

				real t_min = 0;
				real t_max = z_t_maxs[zi];
				real e_min = -z_e_maxs[zi];
				real e_max = z_e_maxs[zi];

				if (modes[mi].mc_dir_tag == "norm")
					{ e_min = 0; e_max = +0.15; e_Step = 0.05; e_step = 0.01; }

				currentpad.xTicks = LeftTicks(t_Step, t_step);
				currentpad.yTicks = RightTicks(e_Step, e_step);
			
				limits((0, -e_max), (t_max, e_max), Crop);
			
				xaxis(YEquals(0, false), dashed);
				yaxis(XEquals(8e-4, false), dashed);
		
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
