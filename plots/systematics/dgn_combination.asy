import root;
import pad_layout;
import common_code;

string topDir = "../../";

string datasets[] = { "DS-fill5313" };

string objects[] = {
	"g_eff_45b",
	"g_eff_45t",
	"g_eff_comb1",
	"g_eff_comb2",
};

real z_t_maxs[], z_t_Steps[], z_t_steps[], z_e_maxs[], z_e_Steps[], z_e_steps[];
z_t_maxs.push(0.004); z_t_Steps.push(0.002); z_t_steps.push(0.001); z_e_maxs.push(0.02); z_e_Steps.push(0.005); z_e_steps.push(0.001);
z_t_maxs.push(0.2); z_t_Steps.push(0.05); z_t_steps.push(0.01); z_e_maxs.push(0.02); z_e_Steps.push(0.005); z_e_steps.push(0.001);
z_t_maxs.push(1.0); z_t_Steps.push(0.2); z_t_steps.push(0.1); z_e_maxs.push(0.04); z_e_Steps.push(0.01); z_e_steps.push(0.005);

AddAllModes();

//----------------------------------------------------------------------------------------------------

NewPad(false, 0, -1);

AddToLegend("45 bot -- 56 top", StdPen(1));
AddToLegend("45 top -- 56 bot", StdPen(2));
AddToLegend("combined, mode 1", StdPen(3));
AddToLegend("combined, mode 2", StdPen(4));

AttachLegend();

//----------------------------------------------------------------------------------------------------

bool legend_drawn = false;
int idx = 0;

for (int dsi : datasets.keys)
{
	for (int zi : z_t_maxs.keys)
	{
		++idx;

		//NewPad(false, idx, -1);
		//label("{\SetFontSizesXX "+replace("\vbox{\hbox{"+datasets[dsi]+"}\hbox{"+title+"}}", "_", "\_")+"}");

		for (int mi : modes.keys)
		{
			//write(modes[mi].tag);

			if (idx == 1)
			{
				NewPad(false, 0, mi);
				label("{\SetFontSizesXX " + modes[mi].label + "}");
			}
			
			NewPad("$|t|\ung{GeV^2}$", "systematic effect", idx, mi);
		
			string f = topDir + "systematics/"+datasets[dsi]+"/matrix_numerical_integration.root";

			for (int oi : objects.keys)
			{
				string pth = "contributions/" + modes[mi].num_int_tag + "/" + objects[oi];
				RootObject obj = RootGetObject(f, pth, error=false);
				if (obj.valid)
					draw(obj, "l,d0", StdPen(oi+1)+1pt);
			}
			

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
		}
	}
}

GShipout(vSkip=0pt);
